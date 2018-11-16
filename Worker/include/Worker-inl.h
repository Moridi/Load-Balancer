#ifndef WORKER_INL_H_
#define WORKER_INL_H_

#ifndef WORKER_H_
#error "Worker-inl.h" should be included only in "Worker.h" file.
#endif

#include <iostream>
#include <sstream>

Worker::Worker()
{
}

void Worker::obtain_fields(std::istringstream input_stream)
{
	std::string field_name;

	for (int i = BEGIN; input_stream >> field_name; ++i)
		fields[field_name] = i;
}

void Worker::print()
{
	for (int i = 0; i < matched_goods.size(); ++i)
		std::cout << matched_goods[i];
}

#endif
