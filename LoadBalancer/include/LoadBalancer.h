#ifndef LOAD_BALANCER_H_
#define LOAD_BALANCER_H_

#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <exception>

class BadFieldType : public std::exception {};

class LoadBalancer
{
public:
	typedef std::vector<std::pair<std::string, std::string>>
			VectorOfStringPairs;

	enum class FieldType : uint8_t
	{
		FILTERING_VALUE = 0,
		SORTING_VALUE,
		PROCESS_COUNT,
		DIRECTION
	};

	inline LoadBalancer() noexcept;
	void get_input();
	std::string get_token(std::string line, const int index) const;
	inline std::vector<std::string> tokenize(const std::string& line,
			const char delimiter) const noexcept;
	inline std::string remove_spaces(std::string line) const;
	void print();
	void fill_fields(const std::vector<std::string>& tokens);
	FieldType get_token_type(std::string field_name);

private:
	VectorOfStringPairs fields;
	std::string sorting_value;
	int process_count;
	std::string direction;
};

#include "LoadBalancer-inl.h"
#endif
