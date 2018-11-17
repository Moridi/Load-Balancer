#include "Presenter.h"

#include <fstream>
#include <iostream>

#include <sys/stat.h>

#define NAMED_PIPE "../../WorkerToPresenter"

using namespace std;

void Presenter::obtain_fields(std::istringstream input_stream)
{
	std::string field_name;
	for (int i = BEGIN; input_stream >> field_name; ++i)
		fields[field_name] = i;
}

void Presenter::merge_new_part(int old_size, int new_size)
{
	merge_sort(old_size, new_size);
	merge(BEGIN, old_size - 1, new_size);
}

// This function gets input and sorts them
void Presenter::process()
{
	constexpr int READ_AND_WRITE_PERMISSION = 0666;
	constexpr char FILE_DELIMITER[] = "###########";

	mkfifo(NAMED_PIPE, READ_AND_WRITE_PERMISSION);

	ifstream input_file(NAMED_PIPE);
	string line;

	// Reading the first line which contains file delimiter
	getline(input_file, line);

	while (true)
	{
		if (line != FILE_DELIMITER)
			break;

		// Reading the second line which contains fields name
		getline(input_file, line);
		obtain_fields(istringstream(line));

		int old_size = matched_goods.size();
		while (getline(input_file, line) && line != FILE_DELIMITER)
			matched_goods.push_back(line);

		merge_new_part(old_size, matched_goods.size() - 1);
	}
}

void Presenter::print()
{
	int i;

	if (is_descended)
		for (i = BEGIN; i < matched_goods.size(); ++i)
			cout << matched_goods[i] << endl;
	else
		for (i = matched_goods.size() - 1; i >= 0; --i)
			cout << matched_goods[i] << endl;
}

void Presenter::merge(int begin, int mid, int right)
{
	constexpr char PRICE[] = "price";

	int left_index, right_index, array_index;
	int left_part_size = mid - begin + 1;
	int right_part_size = right - mid;

	std::vector<std::string> left_part(left_part_size);
	std::vector<std::string> right_part(right_part_size);

	for (left_index = BEGIN; left_index < left_part_size; left_index++)
		left_part[left_index] = matched_goods[begin + left_index];

	for (right_index = BEGIN; right_index < right_part_size; right_index++)
		right_part[right_index] = matched_goods[mid + 1 + right_index];

	left_index = BEGIN;
	right_index = BEGIN;
	array_index = begin;

	while (left_index < left_part_size && right_index < right_part_size)
	{
		// Here is comparison according to their prices
		int left_part_element = atoi(get_token(left_part[left_index],
				fields[PRICE]).c_str());
		int right_part_element = atoi(get_token(right_part[right_index],
				fields[PRICE]).c_str());

		if (left_part_element <= right_part_element)
		{
			matched_goods[array_index] = left_part[left_index];
			left_index++;
		}
		else
		{
			matched_goods[array_index] = right_part[right_index];
			right_index++;
		}
		array_index++;
	}

	while (left_index < left_part_size)
	{
		matched_goods[array_index] = left_part[left_index];
		left_index++;
		array_index++;
	}

	while (right_index < right_part_size)
	{
		matched_goods[array_index] = right_part[right_index];
		right_index++;
		array_index++;
	}
}

void Presenter::merge_sort(int begin, int end)
{
	if (begin < end)
	{
		int mid = (end + begin) / 2;
		merge_sort(begin, mid);
		merge_sort(mid + 1, end);

		merge(begin, mid, end);
	}
}

void Presenter::get_sorting_value(int argc, char**& arguments)
{
	constexpr char DESCEND[] = "descend";
	constexpr int NUMBER_OF_FILE_DESCRIPTORS = 2;

	if (argc < 3)
		throw BadArgumentPassing();

	// argv = [ Presenter + Read File Descriptor + Write File Descriptor ]
	int index = 1;
	int file_descriptor[NUMBER_OF_FILE_DESCRIPTORS];

	file_descriptor[READ_DESCRIPTOR] = atoi(arguments[index++]);
	file_descriptor[WRITE_DESCRIPTOR] = atoi(arguments[index++]);

	string sorting_value = read_from_pipe(file_descriptor);

	if (sorting_value == DESCEND)
		is_descended = true;
	else
		is_descended = false;
}
