#ifndef PRESENTER_H_
#define PRESENTER_H_

#include <vector>
#include <string>
#include <map>
#include <exception>

class BadArgumentPassing : public std::exception {};

class Presenter
{
public:
	inline Presenter();
	inline std::vector<std::string> tokenize(const std::string& line,
			const char delimiter) noexcept;
	inline std::string get_token(std::string line, const int index);

	void obtain_fields(std::istringstream input_stream);
	void print();
	void merge_sort(int begin, int end);
	void merge(int begin, int mid, int end);
	void process();
	void merge_new_part(int old_size, int new_size);
	void get_sorting_value(int argc, char**& argv);

private:
	static constexpr int BEGIN = 0;
	bool is_descended;
	std::vector<std::string> matched_goods;
	std::map<std::string, int> fields;
};

#include "Presenter-inl.h"
#endif
