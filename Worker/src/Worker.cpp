#include "Worker.h"

#include <fcntl.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>

#include <fstream>

#include <sys/stat.h>

using namespace std;

void Worker::filter(string input_string)
{
	constexpr char END_LINE[] = "\n";

	istringstream input_stream(input_string);
	vector<string> good_information;
	string field;

	while (input_stream >> field)
		good_information.push_back(field);

	bool is_matched = true;
	for (int filter_index = 0; filter_index < filters.size();
			++filter_index)
	{
		int field_index = fields[filters[filter_index].first];
		string field_value = filters[filter_index].second;

		if (good_information[field_index] != field_value)
			is_matched = false;
	}

	if (is_matched)
		matched_goods.push_back(input_string + END_LINE);
}

void Worker::read_from_file()
{
	constexpr char FILE_DELIMITER[] = "###########\n";
	constexpr char END_LINE[] = "\n";

	for (int file_index = BEGIN; file_index < files_name.size();
			++file_index)
	{
		ifstream input_file(files_name[file_index]);
		string line;

		// Reading the first line which contains field names
		getline(input_file, line);
		obtain_fields(istringstream(line));

		matched_goods.push_back(FILE_DELIMITER + line + END_LINE);

		while (getline(input_file, line))
			filter(line);

		fields.erase(fields.begin(), fields.end());
	}
}

void Worker::set_files_name(char* arguments[])
{
	constexpr int NUMBER_OF_FILE_DESCRIPTORS = 2;

	const int filters_size = (atoi(arguments[1]) + 1) * 2;
	int file_descriptor[NUMBER_OF_FILE_DESCRIPTORS];

	int number_of_files = atoi(arguments[filters_size]);
	file_descriptor[READ_DESCRIPTOR] = atoi(arguments[filters_size + 1]);
	file_descriptor[WRITE_DESCRIPTOR] = atoi(arguments[filters_size + 2]);

	read_from_pipe(BEGIN, number_of_files, file_descriptor);
}

void Worker::set_filters(char* arguments[])
{
	constexpr int NUMBER_OF_PAIRS_INDEX = 1;

	for (int i = 1; i <= atoi(arguments[NUMBER_OF_PAIRS_INDEX]); ++i)
		filters.push_back(std::pair<std::string, std::string>(
				arguments[i * 2], arguments[i * 2 + 1]));
}

void Worker::send_data_to_presenter()
{
	constexpr int READ_AND_WRITE_PERMISSION = 0666;
	constexpr char myfifo[] = "../../WorkerToPresenter";

	int file_descriptor;

	// mkfifo(<pathname>, <permission>)
	mkfifo(myfifo, READ_AND_WRITE_PERMISSION);
	file_descriptor = open(myfifo, O_WRONLY | O_APPEND);

	for (int i = 0; i < matched_goods.size(); ++i)
		write(file_descriptor, matched_goods[i].c_str(),
				matched_goods[i].size());
	close(file_descriptor);
}

void Worker::read_from_pipe(int begin, int end, int file_descriptor[])
{
	constexpr int MAX_PATH_SIZE = PATH_MAX + 1;

	close(file_descriptor[WRITE_DESCRIPTOR]);

	char* received_value = reinterpret_cast<char*>(
			malloc(sizeof(char) * MAX_PATH_SIZE));

	for (int j = begin; j < end; ++j)
	{
		read(file_descriptor[READ_DESCRIPTOR], received_value,
				MAX_PATH_SIZE);
		files_name.push_back(string(received_value));
	}
	close(file_descriptor[READ_DESCRIPTOR]);
}
