#ifndef PRESENTER_INL_H_
#define PRESENTER_INL_H_

#ifndef PRESENTER_H_
#error "Presenter-inl.h" should be included only in "Presenter.h" file.
#endif

#include <sstream>

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

#endif
