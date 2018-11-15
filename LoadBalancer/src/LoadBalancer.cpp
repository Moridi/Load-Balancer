#include "LoadBalancer.h"

using namespace std;

string LoadBalancer::get_token(string line, const int index) const
{
	constexpr char ASSIGN_DELIMITER = '=';

	std::vector<std::string> tokens = tokenize(line, ASSIGN_DELIMITER);
	return tokens[index];
}

void LoadBalancer::print()
{
	cout << "Fields :" << endl;
	for (int i = 0; i < fields.size(); ++i)
		cout << fields[i].first << ":" << fields[i].second << endl;

	cout << "Sorting Value : " << endl << sorting_value << endl;
	cout << "Process Count : " << endl << process_count << endl;
	cout << "Direction : " << endl << direction << endl;
}

LoadBalancer::FieldType LoadBalancer::get_token_type(string field_name)
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

void LoadBalancer::fill_fields(const vector<string>& tokens)
{
	constexpr int FIELD_NAME_INDEX = 0;
	constexpr int FIELD_VALUE_INDEX = 1;
	constexpr int BEGIN = 0;

	for (int index = BEGIN; index < tokens.size(); ++index)
	{
		cout << "field name and price: " << tokens[index] << endl;
		string field_name = get_token(tokens[index], FIELD_NAME_INDEX);
		string field_value = get_token(tokens[index], FIELD_VALUE_INDEX);

		switch (get_token_type(field_name))
		{
			case FieldType::FILTERING_VALUE:
				fields.push_back(pair<string, string>(field_name, field_value));
				break;
			case FieldType::SORTING_VALUE:
				sorting_value = field_value;
				break;
			case FieldType::PROCESS_COUNT:
				process_count = atoi(field_value.c_str());
				break;
			case FieldType::DIRECTION:
				direction = field_value;
				break;
		}
	}
}

void LoadBalancer::get_input()
{
	constexpr char ARGUMENTS_DELIMITER = '-';
	string input;
	getline(cin, input);

	std::vector<std::string> tokens = tokenize(input, ARGUMENTS_DELIMITER);
	fill_fields(tokens);
}
