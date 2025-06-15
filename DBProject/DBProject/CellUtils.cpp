#include "CellUtils.h"
#include "Date.h"
int toInt(const std::string& token) {
	int value = 0;
	size_t index = (token[0] == '+' || token[0] == '-') ? 1 : 0;
	bool negative = (index == 1 && token[0] == '-');
	for (; index < token.size(); index++) {
		value = value * 10 + token[index] - '0';
	}
	return negative ? -value : value;
}
double toDouble(const std::string& token) {
	std::size_t index = 0;
	bool negative = false;
	if (token[index] == '+' || token[index] == '-') {
		negative = token[index] == '-';
		++index;
	}
	int intPart = 0;
	double fraction = 0.0;
	double division = 1.0;
	bool dot = false;
	for (; index < token.size(); index++) {
		if (token[index] == '.') {
			dot = true;
			continue;
		}
		if (!dot) {
			intPart = intPart * 10 + (token[index] - '0');
		}
		else {
			fraction = fraction * 10 + (token[index] - '0');
			division *= 10.0;
		}
	}
	double result = intPart + fraction / division;
	return negative ? -result : result;
}

bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

std::string unescape(const std::string& line) {
	std::string out;
	for (size_t i = 0; i < line.size(); i++) {
		if (line[i] == '\\' && i + 1 < line.size()) {
			out.push_back(line[++i]);
		}
		else {
			out.push_back(line[i]);
		}
	}
	return out;
}

char lowerChar(char letter){
	if ('A' <= letter && letter <= 'Z') {
		return letter + 32;
	}
	return letter;
}

std::string stripQuotes(const std::string& line) {
	if (line.size() >= 2 && line.front() == '"' && line.back() == '"') {
		return line.substr(1, line.size() - 2);
	}
	return line;
}

