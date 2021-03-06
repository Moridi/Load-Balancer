#ifndef LOAD_BALANCER_H_
#define LOAD_BALANCER_H_

#include <limits.h>
#include <dirent.h>

#include <string>
#include <vector>
#include <exception>

class BadFieldType : public std::exception {};
class BadDirectoryAddress : public std::exception {};

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
	inline std::string get_token(std::string line, const int index) const;
	inline std::vector<std::string> tokenize(const std::string& line,
			const char delimiter) const noexcept;
	inline std::string remove_spaces(std::string line) const;
	inline FieldType get_token_type(std::string field_name);
	inline bool is_new_file(std::string entry);
	inline std::string get_full_path(std::string entry);
	inline void set_file_arguments(char*** argv, int& index,
			const std::vector<std::string>& files_name);
	inline void set_argv_element(char*** argv, int index,
			std::string value);
	inline size_t get_size(std::string value);
	inline void wait_for_all_workers();
	inline void set_presenter_arguments(char*** argv,
			int file_descriptor[]);
	inline void set_file_descriptor_arguments(char*** argv,
			int file_descriptor[], int& index, int number_of_files);
	inline void set_argv(char*** argv, int file_descriptor[],
			int number_of_files);
	inline void write_to_pipe(std::string data, int file_descriptor[]);
	void clear_data();

	void get_input();
	void fill_fields(const std::vector<std::string>& tokens);
	void iterate_on_directory();
	void process();
	void send_data_to_worker(int index, int files_per_worker);
	void set_filter_arguments(char*** argv, int& index);
	void write_dataset_to_pipe(int begin, int end, int file_descriptor[]);
	void exec_worker(int file_descriptor[], int number_of_files);
	void fill_dataset(DIR* directory);
	void setup_presenter();

private:
	static constexpr uint8_t BEGIN = 0;
	static constexpr uint8_t READ_DESCRIPTOR = 0;
	static constexpr uint8_t WRITE_DESCRIPTOR = 1;
	static constexpr int MAX_PATH_SIZE = PATH_MAX + 1;

	VectorOfStringPairs fields;
	std::string sorting_value;
	int process_count;
	std::string dataset_directory;
	std::vector<std::string> dataset;
	bool process_is_done;
};

#include "LoadBalancer-inl.h"
#endif
