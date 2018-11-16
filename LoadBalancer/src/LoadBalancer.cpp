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

string LoadBalancer::get_token(string line, const int index) const
{
	constexpr char ASSIGN_DELIMITER = '=';

	std::vector<std::string> tokens = tokenize(line, ASSIGN_DELIMITER);
	return tokens[index];
}

void LoadBalancer::print()
{
	cout << "Fields :" << endl;
	for (int i = 0; i < fields.size(); ++i)
		cout << fields[i].first << ":" << fields[i].second << endl;

	cout << "Sorting Value : " << endl << sorting_value << endl;
	cout << "Process Count : " << endl << process_count << endl;
	cout << "Direction : " << endl << dataset_directory << endl;

	cout << "Data Set:" << endl;
	for (int i = 0; i < dataset.size(); ++i)
		cout << dataset[i] << endl;
}

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

	for (int index = BEGIN; index < tokens.size(); ++index)
	{
		string field_name = get_token(tokens[index], FIELD_NAME_INDEX);
		string field_value = get_token(tokens[index], FIELD_VALUE_INDEX);

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

int LoadBalancer::iterate_on_directory()
{
	DIR* directory;
	struct dirent* entry;

	if ((directory = opendir(dataset_directory.c_str())) != nullptr)
	{
		while ((entry = readdir(directory)) != nullptr)
			if (is_new_file(string(entry->d_name)))
			{
				string full_path = get_full_path(
						string(entry->d_name));
				dataset.push_back(full_path);
			}
		closedir (directory);
	}
	else
	{
		cerr << "Could not open directory..." << endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void LoadBalancer::set_argv(vector<string> files_name, char*** argv)
{
	constexpr char worker[] = "Worker";
	int i;
	int j = 0;

	(*argv)[0] = reinterpret_cast<char*>(malloc(sizeof(worker)));
	strcpy((*argv)[0], worker);

	string number_of_fields = to_string(fields.size());

	(*argv)[1] = reinterpret_cast<char*>(malloc(
			number_of_fields.size() * sizeof(char)));
	strcpy((*argv)[1], number_of_fields.c_str());

	for (i = 2; j < fields.size(); i += 2)
	{
		(*argv)[i] = reinterpret_cast<char*>(malloc(
				fields[j].first.size() * sizeof(char)));
		strcpy((*argv)[i], fields[j].first.c_str());

		(*argv)[i + 1] = reinterpret_cast<char*>(malloc(
				fields[j].second.size() * sizeof(char)));
		strcpy((*argv)[i + 1], fields[j].second.c_str());
		++j;
	}

	for (j = 0; j < files_name.size(); ++i)
	{
		(*argv)[i] = reinterpret_cast<char*>(malloc(
				files_name[j].size() * sizeof(char)));
		strcpy((*argv)[i], files_name[j].c_str());
		j++;
	}

	(*argv)[i] = nullptr;
}

void LoadBalancer::send_data(int begin, int end)
{
	constexpr int CHILD = 0;
	constexpr int READ_DESCRIPTOR = 0;
	constexpr int WRITE_DESCRIPTOR = 1;
	constexpr int MAX_PATH_SIZE = PATH_MAX + 1;

	int file_descriptor[2];
	char* received_value = reinterpret_cast<char*>(
			malloc(sizeof(char) * MAX_PATH_SIZE));

	pipe(file_descriptor);

	if (fork() == CHILD)
	{
		close(file_descriptor[WRITE_DESCRIPTOR]);
		vector<string> files_name;

		for (int j = begin; j < end; ++j)
		{
			read(file_descriptor[READ_DESCRIPTOR], received_value,
					MAX_PATH_SIZE);
			files_name.push_back(string(received_value));
		}

		cout << "\nChild with PID: " << getpid() << endl;

		close(file_descriptor[READ_DESCRIPTOR]);
//		char** argv = reinterpret_cast<char**>(malloc(
//				files_name.size() * sizeof(char*) + 2));

		char** argv = reinterpret_cast<char**>(malloc(
				(files_name.size() + fields.size() * 2 + 2) * sizeof(char*)));
		set_argv(files_name, &argv);
		execv("../../Worker/builds/Worker", argv);
		exit(EXIT_SUCCESS);
	}
	else
	{
		close(file_descriptor[READ_DESCRIPTOR]);
		cout << "Parent with PID: " << getpid() << endl;

		for (int j = begin; j < end; ++j)
		{
			write(file_descriptor[WRITE_DESCRIPTOR], dataset[j].c_str(),
			      MAX_PATH_SIZE);
			cout << dataset[j] << endl;
		}
		close(file_descriptor[WRITE_DESCRIPTOR]);
		wait(nullptr);
	}
}

void LoadBalancer::allot_files()
{
	int files_per_worker = dataset.size() / process_count;
	int i;

	for (i = BEGIN; i + files_per_worker < dataset.size(); i += files_per_worker)
	{
		if (i / files_per_worker + 1 == process_count)
			break;
		send_data(i, i + files_per_worker);
	}

	send_data(i, dataset.size());
}
