#pragma once
#include <string>
enum class CellType { INVALID = -1, Integer, Double, Date, String, COUNT };
int toInt(const std::string& token);
double toDouble(const std::string& token);
bool isDigit(char c);
