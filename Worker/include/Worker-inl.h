#ifndef WORKER_INL_H_
#define WORKER_INL_H_

#ifndef WORKER_H_
#error "Worker-inl.h" should be included only in "Worker.h" file.
#endif

#include <iostream>
#include <sstream>

Worker::Worker()
{}

void Worker::obtain_fields(std::istringstream input_stream)
{
	std::string field_name;

	for (int i = BEGIN; input_stream >> field_name; ++i)
		fields[field_name] = i;
}

void Worker::set_file_names()
{
	std::string field_name;
	while((std::cin >> field_name) && (field_name != "quit"))
		file_names.push_back(field_name);
}

void Worker::set_filters()
{
	std::string field_name, field_value;
	while((std::cin >> field_name >> field_value) && (field_name != "quit"))
		filters.push_back(std::pair<std::string, std::string>(
				field_name, field_value));
}

#endif
