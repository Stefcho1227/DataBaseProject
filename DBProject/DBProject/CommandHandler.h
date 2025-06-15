#pragma once
#include "Database.h"
#include <string>
class CommandHandler {
private:
	DataBase db;
	void splitFirst(const std::string& line, std::string& command, std::string& args) const;
	bool execute(const std::string& command, const std::string& args);
	std::vector<std::string> splitArgs(const std::string& literals) const;
	std::string trim(const std::string& line) const;
	std::vector<std::string> splitArgsWithSpaces(const std::string& literals);
public:
	void run();
};