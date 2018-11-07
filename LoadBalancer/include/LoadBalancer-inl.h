#ifndef LOAD_BALANCER_INL_H_
#define LOAD_BALANCER_INL_H_

#ifndef LOAD_BALANCER_H_
#error "LoadBalancer-inl.h" should be included only in "LoadBalancer.h" file.
#endif

#include <bits/stdc++.h>

LoadBalancer::LoadBalancer() noexcept
{}

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

#endif
