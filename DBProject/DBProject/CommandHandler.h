#pragma once
#include "Database.h"
#include <string>
class CommandHandler {
private:
	DataBase db;
	void splitFirst(const std::string& line, std::string& command, std::string& args) const;
	bool execute(const std::string& command, const std::string& args);
public:
	void run();
};