#include "Worker.h"

#include <fstream>

using namespace std;

void Worker::filter(string input_string)
{
	istringstream input_stream(input_string);
	vector<string> good_information;
	string field;

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
	for (int file_index = BEGIN; file_index < files_name.size(); ++file_index)
	{
		ifstream input_file(files_name[file_index]);
		string line;

		// Reading the first line which contains field names
		getline(input_file, line);
		obtain_fields(istringstream(line));

		while (getline(input_file, line))
			filter(line);

		fields.erase(fields.begin(), fields.end());
	}
}

void Worker::set_files_name(int size, char* arguments[])
{
	int filters_size = (atoi(arguments[1]) + 1) * 2;
	size = size - filters_size;

	for (int i = filters_size, j = 0; j < size; ++i, ++j)
		files_name.push_back(string(arguments[i]));
}

void Worker::set_filters(char* arguments[])
{
	constexpr int NUMBER_OF_PAIRS_INDEX = 1;

	for (int i = 1; i <= atoi(arguments[NUMBER_OF_PAIRS_INDEX]); ++i)
		filters.push_back(std::pair<std::string, std::string>(
				arguments[i * 2], arguments[i * 2 + 1]));
}
