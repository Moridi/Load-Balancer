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
	void read_from_file();
	void filter(std::istringstream input_stream);
	inline void obtain_fields(std::istringstream input_stream);
	void set_filters();
	void set_file_names();
//	inline int find(std::string field);

private:
	static constexpr int BEGIN = 0;
	std::vector<std::string> file_names;
	std::map<std::string, int> fields;
	VectorOfStringPairs filters;
};

#include "Worker-inl.h"
#endif
