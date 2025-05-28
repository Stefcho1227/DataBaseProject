#pragma once
#include <string>
#include <stdexcept>
class Date {
private:
	int day;
	int month;
	int year;
	static bool isLeapYear(int year);
	static int monthLenght(int month, int year);
public:
	Date();
	Date(int day, int month, int year);
	static bool isValid(int day, int month, int year);
	void setDate(int day, int month, int year);
	static Date parse(const char* string);
	std::string str() const;
	int getDay() const;
	int getMonth() const;
	int getYear() const;
};
