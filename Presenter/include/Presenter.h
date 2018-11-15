#ifndef PRESENTER_H_
#define PRESENTER_H_

#include <vector>
#include <string>
#include <map>

class Presenter
{
public:
	Presenter(std::string file_name);
	void obtain_fields(std::istringstream input_stream);
	void print();
	void sort();
	void merge_sort(int l, int r);
	void merge(int l, int m, int r);
	void print_array();
	inline std::vector<std::string> tokenize(const std::string& line,
			const char delimiter) noexcept;
	std::string get_token(std::string line, const int index);
private:
	static constexpr int BEGIN = 0;
	bool is_descended;
	std::vector<std::string> matched_goods;
	std::map<std::string, int> fields;
};

#include "Presenter-inl.h"
#endif
