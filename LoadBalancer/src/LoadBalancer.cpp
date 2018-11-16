#include "LoadBalancer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <strings.h>

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

void LoadBalancer::send_data(string file_data)
{
	constexpr int READ_DESCRIPTOR = 0;
	constexpr int WRITE_DESCRIPTOR = 1;
	constexpr int MAX_PATH_SIZE = PATH_MAX + 1;
	int file_descriptor[2];
	char* received_value = reinterpret_cast<char*>(
			malloc(sizeof(char) * MAX_PATH_SIZE));

	pipe(file_descriptor);

	if (fork() == 0)
	{
		close(file_descriptor[WRITE_DESCRIPTOR]);
		read(file_descriptor[READ_DESCRIPTOR], received_value,
				MAX_PATH_SIZE);
		printf("Child(%d) received value: %s\n", getpid(), received_value);

		close(file_descriptor[READ_DESCRIPTOR]);
	}
	else
	{
		close(file_descriptor[READ_DESCRIPTOR]);
		write(file_descriptor[WRITE_DESCRIPTOR], file_data.c_str(),
				MAX_PATH_SIZE);
		printf("Parent(%d) send value: %s\n", getpid(), file_data.c_str());

		close(file_descriptor[WRITE_DESCRIPTOR]);
	}
}

void LoadBalancer::allot_files()
{
	int files_per_worker = dataset.size() / process_count;
	int i, j;

	for (i = files_per_worker; i < dataset.size(); i += files_per_worker)
	{
		cout << i << "- Worker:" << endl;
		for (j = i - files_per_worker; j < i; ++j)
			cout << dataset[j] << endl;
		if (i / files_per_worker != process_count)
			cout << "it's not last one!" << endl;
	}

	for (; j < dataset.size(); ++j)
		send_data(dataset[j]);
}
