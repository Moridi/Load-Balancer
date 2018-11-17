#ifndef LOAD_BALANCER_INL_H_
#define LOAD_BALANCER_INL_H_

#ifndef LOAD_BALANCER_H_
#error "LoadBalancer-inl.h" should be included only in "LoadBalancer.h" file.
#endif

#include <unistd.h>

#include <bits/stdc++.h>
#include <sys/wait.h>

LoadBalancer::LoadBalancer() noexcept
: process_is_done(false)
{
}

std::string LoadBalancer::remove_spaces(std::string line) const
{
	constexpr char SPACE = ' ';

	std::string::iterator end_position = std::remove(line.begin(),
			line.end(), SPACE);
	line.erase(end_position, line.end());
	return line;
}

std::vector<std::string> LoadBalancer::tokenize(const std::string& line,
		const char delimiter) const noexcept
{
	std::vector<std::string> tokens;
	std::stringstream string_stream(line);
	std::string intermediate;
	while (getline(string_stream, intermediate, delimiter))
		tokens.push_back(remove_spaces(intermediate));

	return tokens;
}

bool LoadBalancer::is_new_file(std::string entry)
{
	constexpr char CURRENT_DIRECTORY[] = ".";
	constexpr char PARENT_DIRECTORY[] = "..";

	return entry != CURRENT_DIRECTORY && entry != PARENT_DIRECTORY;
}

std::string LoadBalancer::get_full_path(std::string entry)
{
	constexpr char CURRENT_DIRECTORY[] = ".";
	constexpr char SLASH[] = "/";

	char current_path[PATH_MAX + 1];
	realpath(CURRENT_DIRECTORY, current_path);

	return std::string(current_path) + SLASH + dataset_directory +
			SLASH + entry;
}

size_t LoadBalancer::get_size(std::string value)
{
	return value.size() * static_cast<size_t>(sizeof(char));
}

void LoadBalancer::set_argv_element(char*** argv, int index, std::string value)
{
	(*argv)[index] = reinterpret_cast<char*>(malloc(get_size(value)));
	strcpy((*argv)[index], value.c_str());
}

void LoadBalancer::set_file_arguments(char*** argv, int& index,
		const std::vector<std::string>& files_name)
{
	for (int j = 0; j < files_name.size(); ++index, ++j)
		set_argv_element(argv, index, files_name[j]);
}

std::string LoadBalancer::get_token(std::string line, const int index) const
{
	constexpr char ASSIGN_DELIMITER = '=';

	std::vector<std::string> tokens = tokenize(line, ASSIGN_DELIMITER);
	return tokens[index];
}

void LoadBalancer::wait_for_all_workers()
{
	while(wait(nullptr) > 0);
}

void LoadBalancer::set_file_descriptor_arguments(char*** argv,
		int file_descriptor[], int& index, int number_of_files)
{
	set_argv_element(argv, index++, std::to_string(number_of_files));
	set_argv_element(argv, index++, std::to_string(
			file_descriptor[READ_DESCRIPTOR]));
	set_argv_element(argv, index++, std::to_string(
			file_descriptor[WRITE_DESCRIPTOR]));
}

void LoadBalancer::set_argv(char*** argv, int file_descriptor[],
		int number_of_files)
{
	int index;
	set_filter_arguments(argv, index);
	set_file_descriptor_arguments(argv, file_descriptor, index,
			number_of_files);
	(*argv)[index] = nullptr;
}

LoadBalancer::FieldType LoadBalancer::get_token_type(std::string field_name)
{
	constexpr char PRICE[] = "price";
	constexpr char PROCESS_COUNT[] = "prc_cnt";
	constexpr char DIRECTION[] = "dir";

	if (field_name == PRICE)
		return FieldType::SORTING_VALUE;
	else if (field_name == PROCESS_COUNT)
		return FieldType::PROCESS_COUNT;
	else if (field_name == DIRECTION)
		return FieldType::DIRECTION;
	else
		return FieldType::FILTERING_VALUE;
}

void LoadBalancer::set_presenter_arguments(char*** argv, int file_descriptor[])
{
	// argv = [ Presenter + Read File Descriptor + Write File Descriptor ]

	constexpr int PRESENTER_ARGUMENT_SIZE = 4;
	constexpr char PRESENTER[] = "Presenter";
	int index = 0;

	(*argv) = reinterpret_cast<char**>(malloc(PRESENTER_ARGUMENT_SIZE *
			static_cast<size_t>(sizeof(char*))));

	set_argv_element(argv, index++, PRESENTER);
	set_argv_element(argv, index++, std::to_string(
			file_descriptor[READ_DESCRIPTOR]));
	set_argv_element(argv, index++, std::to_string(
			file_descriptor[WRITE_DESCRIPTOR]));
	(*argv)[index++] = nullptr;
}

void LoadBalancer::write_to_pipe(std::string data, int file_descriptor[])
{
	close(file_descriptor[READ_DESCRIPTOR]);
	write(file_descriptor[WRITE_DESCRIPTOR], data.c_str(), MAX_PATH_SIZE);
	close(file_descriptor[WRITE_DESCRIPTOR]);
}

#endif
