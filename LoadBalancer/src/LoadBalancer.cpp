#include "LoadBalancer.h"

#include <unistd.h>

#define EXECUTABLE_PRESENTER "../../Presenter/builds/Presenter"
#define EXECUTABLE_WORKER "../../Worker/builds/Worker"

using namespace std;

void LoadBalancer::fill_fields(const vector<string>& tokens)
{
	constexpr int FIELD_NAME_INDEX = 0;
	constexpr int FIELD_VALUE_INDEX = 1;
	constexpr int BEGIN = 0;

	string field_name;
	string field_value;

	for (int index = BEGIN; index < tokens.size(); ++index)
	{
		field_name = get_token(tokens[index], FIELD_NAME_INDEX);
		field_value = get_token(tokens[index], FIELD_VALUE_INDEX);

		switch (get_token_type(field_name))
		{
			case FieldType::FILTERING_VALUE:
				fields.push_back(pair<string, string>(
						field_name, field_value));
				break;
			case FieldType::SORTING_VALUE:
				sorting_value = field_value;
				break;
			case FieldType::PROCESS_COUNT:
				process_count = atoi(field_value.c_str());
				break;
			case FieldType::DIRECTION:
				dataset_directory = field_value;
				break;
		}
	}
}

void LoadBalancer::get_input()
{
	constexpr char ARGUMENTS_DELIMITER = '-';

	string input;
	getline(cin, input);
	std::vector<std::string> tokens = tokenize(input, ARGUMENTS_DELIMITER);
	fill_fields(tokens);
}

void LoadBalancer::fill_dataset(DIR* directory)
{
	struct dirent* entry;
	while ((entry = readdir(directory)) != nullptr)
	if (is_new_file(string(entry->d_name)))
	{
		string full_path = get_full_path(string(entry->d_name));
		dataset.push_back(full_path);
	}
	closedir(directory);
}

void LoadBalancer::iterate_on_directory()
{
	DIR* directory;

	if ((directory = opendir(dataset_directory.c_str())) != nullptr)
		fill_dataset(directory);
	else
	{
		cerr << "Could not open directory..." << endl;
		throw BadDirectoryAddress();
	}
}

void LoadBalancer::set_filter_arguments(char*** argv, int& index)
{
	constexpr char worker[] = "Worker";
	constexpr int EXECUTABLE_INDEX = 0;
	constexpr int NUMBER_OF_FIELDS_INDEX = 1;

	int j = 0;

	set_argv_element(argv, EXECUTABLE_INDEX, worker);
	set_argv_element(argv, NUMBER_OF_FIELDS_INDEX, to_string(
			fields.size()));

	for (index = 2; j < fields.size(); index += 2, ++j)
	{
		set_argv_element(argv, index, fields[j].first);
		set_argv_element(argv, index + 1, fields[j].second);
	}
}

void LoadBalancer::exec_worker(int file_descriptor[], int number_of_files)
{
	// argv = [Command + Number of Fields + (Fields(pair))* +
	// Number of Files + First File Descriptor + Second File Descriptor]
	constexpr int MINIMUM_NUMBER_OF_ARGUMENTS = 5;
	constexpr int PAIR = 2;

	size_t argv_size = fields.size() * PAIR + MINIMUM_NUMBER_OF_ARGUMENTS;
	char** argv = reinterpret_cast<char**>(malloc(
			argv_size * static_cast<size_t>(sizeof(char*))));

	set_argv(&argv, file_descriptor, number_of_files);
	execv(EXECUTABLE_WORKER, argv);
}

void LoadBalancer::write_to_pipe(int begin, int end, int file_descriptor[])
{
	close(file_descriptor[READ_DESCRIPTOR]);

	for (int j = begin; j < end; ++j)
		write(file_descriptor[WRITE_DESCRIPTOR], dataset[j].c_str(),
				MAX_PATH_SIZE);

	close(file_descriptor[WRITE_DESCRIPTOR]);
}

void LoadBalancer::send_data_to_worker(int begin, int end)
{
	constexpr int CHILD = 0;
	constexpr int NUMBER_OF_FILE_DESCRIPTORS = 2;

	int file_descriptor[NUMBER_OF_FILE_DESCRIPTORS];
	pipe(file_descriptor);

	if (fork() == CHILD)
		exec_worker(file_descriptor, end - begin);

	write_to_pipe(begin, end, file_descriptor);
}

void LoadBalancer::setup_presenter()
{
	char** argv;
	set_presenter_arguments(&argv);
	if (fork() == 0)
		execv(EXECUTABLE_PRESENTER, argv);

	wait(nullptr);
}

void LoadBalancer::allot_files()
{
	int files_per_worker = dataset.size() / process_count;
	int i;

	for (i = BEGIN; i + files_per_worker < dataset.size();
			i += files_per_worker)
	{
		if (i / files_per_worker + 1 == process_count)
			break;
		send_data_to_worker(i, i + files_per_worker);
	}
	send_data_to_worker(i, dataset.size());
	wait_for_all_workers();
	setup_presenter();
}
