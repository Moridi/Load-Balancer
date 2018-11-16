#ifndef LOAD_BALANCER_H_
#define LOAD_BALANCER_H_

#include <string>
#include <vector>
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
	int iterate_on_directory();
	inline bool is_new_file(std::string entry);
	inline std::string get_full_path(std::string entry);
	void allot_files();
	void send_data(int index, int files_per_worker);
	void set_argv(std::vector<std::string> files_name, char*** argv);

private:
	static constexpr int BEGIN = 0;
	VectorOfStringPairs fields;
	std::string sorting_value;
	int process_count;
	std::string dataset_directory;
	std::vector<std::string> dataset;
};

#include "LoadBalancer-inl.h"
#endif
