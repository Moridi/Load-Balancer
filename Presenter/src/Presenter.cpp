#include "Presenter.h"

#include <stdlib.h>

#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

void Presenter::obtain_fields(std::istringstream input_stream)
{
	std::string field_name;

	for (int i = BEGIN; input_stream >> field_name; ++i)
		fields[field_name] = i;
}

Presenter::Presenter(string file_name)
{
	constexpr char DESCEND[] = "descend";

	ifstream input_file(file_name);
	string line;

	// Reading the first line which contains sorting type
	getline(input_file, line);
	if (line == DESCEND)
		is_descended = true;
	else
		is_descended = false;

	// Reading the second line which contains field names
	getline(input_file, line);
	obtain_fields(istringstream(line));

	while (getline(input_file, line))
		matched_goods.push_back(line);
}

void Presenter::print()
{
	int i;

	if (is_descended)
		for (i = BEGIN; i < matched_goods.size(); ++i)
			cout << "	" << matched_goods[i] << endl;
	else
		for (i = matched_goods.size() - 1; i >= 0; --i)
			cout << "	" << matched_goods[i] << endl;
}

void Presenter::merge(int begin, int mid, int right)
{
	int left_index, right_index, array_index;
	int left_part_size = mid - begin + 1;
	int right_part_size = right - mid;

	std::vector<std::string> left_part(left_part_size);
	std::vector<std::string> right_part(right_part_size);

	for (left_index = BEGIN; left_index < left_part_size; left_index++)
		left_part[left_index] = matched_goods[begin + left_index];

	for (right_index = BEGIN; right_index < right_part_size; right_index++)
		right_part[right_index] = matched_goods[mid + 1+ right_index];

	left_index = BEGIN;
	right_index = BEGIN;
	array_index = begin;

	while (left_index < left_part_size && right_index < right_part_size)
	{
		int left_part_element = atoi(get_token(left_part[left_index],
				fields["price"]).c_str());
		int right_part_element = atoi(get_token(right_part[right_index],
				fields["price"]).c_str());

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

void Presenter::sort()
{
	merge_sort(BEGIN, matched_goods.size() - 1);
}

std::vector<std::string> Presenter::tokenize(const std::string& line,
		const char delimiter) noexcept
{
	std::vector<std::string> tokens;
	std::stringstream string_stream(line);
	std::string intermediate;
	while (getline(string_stream, intermediate, delimiter))
		tokens.push_back(intermediate);

	return tokens;
}

std::string Presenter::get_token(string line, const int index)
{
	constexpr char ASSIGN_DELIMITER = ' ';
	std::vector<std::string> tokens = tokenize(line, ASSIGN_DELIMITER);
	return tokens[index];
}
