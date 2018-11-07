#include "Worker.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

void Worker::set_file_names()
{
	string field_name;
	while((cin >> field_name) && (field_name != "quit"))
		file_names.push_back(field_name);
}

void Worker::set_filters()
{
	string field_name, field_value;
	while((cin >> field_name >> field_value) && (field_name != "quit"))
		filters.push_back(pair<string, string>(field_name, field_value));
}

void Worker::filter(istringstream input_stream)
{
	string field;
	vector<string> good_information;
	while (input_stream >> field)
		good_information.push_back(field);


	bool is_matched = true;
	for (int filter_index = 0; filter_index < filters.size(); ++filter_index)
	{
		int field_index = fields[filters[filter_index].first];
		string field_value = filters[filter_index].second;

		if (good_information[field_index] != field_value)
			is_matched = false;
	}

	if (is_matched)
	{
		for (int i = 0; i < good_information.size(); ++i)
			cout << good_information[i] << "*";
		cout << endl;
	}
}

void Worker::read_from_file()
{
	for (int file_index = BEGIN; file_index < file_names.size(); ++file_index)
	{
		ifstream input_file(file_names[file_index]);
		string line;

		// Reading the first line which contains field names
		getline(input_file, line);
		obtain_fields(istringstream(line));

		while (getline(input_file, line))
			filter(istringstream(line));

		fields.erase(fields.begin(), fields.end());
	}
}
