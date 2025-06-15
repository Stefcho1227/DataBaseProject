#pragma once
#include <string>
enum class CellType { INVALID = -1, Integer, Double, Date, String, COUNT };
int toInt(const std::string& token);
double toDouble(const std::string& token);
bool isDigit(char c);
std::string unescape(const std::string& line);
char lowerChar(char letter);
std::string stripQuotes(const std::string& line);