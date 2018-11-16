#include "LoadBalancer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <strings.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

LoadBalancer::FieldType LoadBalancer::get_token_type(string field_name)
{
	constexpr char PRICE[] = "price";
	constexpr char PROCESS_COUNT[] = "prc_cnt";
	constexpr char DIRECTION[] = "dir";

	if (field_name == PRICE)
		return FieldType::SORTING_VALUE;
	else if (field_name == PROCESS_COUNT)
		return FieldType::PROCESS_COUNT;
	else if (field_name == DIRECTION)
		return FieldType::DIRECTION;
	else
		return FieldType::FILTERING_VALUE;
}

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

void LoadBalancer::read_from_pipe(int begin, int end,
		vector<string>& files_name, int file_descriptor[])
{
	char* received_value = reinterpret_cast<char*>(
			malloc(sizeof(char) * MAX_PATH_SIZE));

	for (int j = begin; j < end; ++j)
	{
		read(file_descriptor[READ_DESCRIPTOR], received_value,
				MAX_PATH_SIZE);
		files_name.push_back(string(received_value));
	}
}

void LoadBalancer::exec_worker(const vector<string>& files_name,
		int file_descriptor[])
{
	static constexpr char EXECUTABLE_WORKER[] =
			"../../Worker/builds/Worker";

	size_t argv_size = files_name.size() + fields.size() * 2 + 2;
	close(file_descriptor[READ_DESCRIPTOR]);
	char** argv = reinterpret_cast<char**>(malloc(
			argv_size * static_cast<size_t>(sizeof(char*))));
	set_argv(files_name, &argv);
	execv(EXECUTABLE_WORKER, argv);
	exit(EXIT_FAILURE);
}

void LoadBalancer::setup_new_worker(int file_descriptor[], int begin, int end)
{
	close(file_descriptor[WRITE_DESCRIPTOR]);
	vector<string> files_name;

	read_from_pipe(begin, end, files_name, file_descriptor);
	exec_worker(files_name, file_descriptor);
}

void LoadBalancer::write_to_pipe(int begin, int end, int file_descriptor[])
{
	close(file_descriptor[READ_DESCRIPTOR]);

	for (int j = begin; j < end; ++j)
		write(file_descriptor[WRITE_DESCRIPTOR], dataset[j].c_str(),
				MAX_PATH_SIZE);

	close(file_descriptor[WRITE_DESCRIPTOR]);
	wait(nullptr);
}

void LoadBalancer::send_data(int begin, int end)
{
	constexpr int CHILD = 0;

	int file_descriptor[2];
	pipe(file_descriptor);

	if (fork() == CHILD)
		setup_new_worker(file_descriptor, begin, end);
	else
		write_to_pipe(begin, end, file_descriptor);

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
		send_data(i, i + files_per_worker);
	}

	send_data(i, dataset.size());
}
