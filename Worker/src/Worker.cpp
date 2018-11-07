#include "Worker.h"

#include <fstream>

using namespace std;

void Worker::filter(string input_string)
{
	istringstream input_stream(input_string);
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
		matched_goods.push_back(input_string);
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
			filter(line);

		fields.erase(fields.begin(), fields.end());
	}
}
