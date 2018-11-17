#ifndef PRESENTER_INL_H_
#define PRESENTER_INL_H_

#ifndef PRESENTER_H_
#error "Presenter-inl.h" should be included only in "Presenter.h" file.
#endif

#include <sstream>
#include <limits.h>
#include <unistd.h>

Presenter::Presenter()
{
}

std::vector<std::string> Presenter::tokenize(const std::string& line,
		const char delimiter) noexcept
{
	std::vector<std::string> tokens;
	std::stringstream string_stream(line);
	std::string intermediate;
	while (getline(string_stream, intermediate, delimiter))
		tokens.push_back(intermediate);

	return tokens;
}

std::string Presenter::get_token(std::string line, const int index)
{
	constexpr char ASSIGN_DELIMITER = ' ';
	std::vector<std::string> tokens = tokenize(line, ASSIGN_DELIMITER);
	return tokens[index];
}

std::string Presenter::read_from_pipe(int file_descriptor[])
{
	constexpr int MAX_PATH_SIZE = PATH_MAX + 1;

	close(file_descriptor[WRITE_DESCRIPTOR]);

	char* received_value = reinterpret_cast<char*>(
			malloc(sizeof(char) * MAX_PATH_SIZE));

	read(file_descriptor[READ_DESCRIPTOR], received_value, MAX_PATH_SIZE);
	close(file_descriptor[READ_DESCRIPTOR]);
	return std::string(received_value);
}

#endif
