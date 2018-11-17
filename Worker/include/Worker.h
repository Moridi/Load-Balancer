#ifndef WORKER_H_
#define WORKER_H_

#include <vector>
#include <string>
#include <map>
#include <exception>

class BadFieldType : public std::exception {};

class Worker
{
public:
	typedef std::vector<std::pair<std::string, std::string>>
			VectorOfStringPairs;

	inline Worker();
	inline void obtain_fields(std::istringstream input_stream);
	void read_from_file();
	void filter(std::string input_string);
	void set_filters(char* arguments[]);
	void set_files_name(char* files_name[]);
	void send_data_to_presenter();
	void obtain_matched_goods(std::ifstream input_file, std::string line);
	void read_from_pipe(int begin, int end, int file_descriptor[]);

private:
	static constexpr uint8_t BEGIN = 0;
	static constexpr uint8_t READ_DESCRIPTOR = 0;
	static constexpr uint8_t WRITE_DESCRIPTOR = 1;

	std::vector<std::string> files_name;
	std::map<std::string, int> fields;
	VectorOfStringPairs filters;
	std::vector<std::string> matched_goods;
};

#include "Worker-inl.h"
#endif
