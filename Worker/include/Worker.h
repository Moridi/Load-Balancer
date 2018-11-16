#ifndef WORKER_H_
#define WORKER_H_

#include <exception>
#include <vector>
#include <string>
#include <map>

class BadFieldType : public std::exception {};

class Worker
{
public:
	typedef std::vector<std::pair<std::string, std::string>>
			VectorOfStringPairs;

	inline Worker();
	inline void obtain_fields(std::istringstream input_stream);
	inline void print();
	void read_from_file();
	void filter(std::string input_string);
	void set_filters(char* arguments[]);
	void set_files_name(int size, char* files_name[]);
	void send_data_to_presenter();

private:
	static constexpr int BEGIN = 0;

	std::vector<std::string> files_name;
	std::map<std::string, int> fields;
	VectorOfStringPairs filters;
	std::vector<std::string> matched_goods;
};

#include "Worker-inl.h"
#endif
