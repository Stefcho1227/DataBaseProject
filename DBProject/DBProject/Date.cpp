#include "Date.h"
#include "CellUtils.h"

bool Date::isLeapYear(int year){
    return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

int Date::monthLenght(int month, int year) {
    static const int dates[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    return month == 2 ? (isLeapYear(year) ? 29 : 28) : dates[month];
}

Date::Date() : day(1), month(1), year(1900) {}

Date::Date(int day, int month, int year) {
    setDate(day, month, year);
}

bool Date::isValid(int day, int month, int year) {
    return year >= 1900 && month >= 1 && month <= 12 && day >= 1 && day <= monthLenght(month, year);
}

void Date::setDate(int day, int month, int year) {
    if (!isValid(day, month, year)) {
        throw std::invalid_argument("Bad date");
    }
    this->day = day;
    this->month = month;
    this->year = year;
}

Date Date::parse(const char* string) {
    int day = 0;
    int month = 0;
    int year = 0;
    size_t index = 0;
    if(!isDigit(string[index])){
        throw std::invalid_argument("Bad date");
    }
    while (isDigit(string[index])) {
        day = day * 10 + (string[index] - '0');
        ++index;
    }
    if (string[index] != '.') {
        throw std::invalid_argument("Bad date");
    }
    ++index;
    if (!isDigit(string[index])) {
        throw std::invalid_argument("Bad date");
    }
    while (isDigit(string[index])) {
        month = month * 10 + (string[index] - '0');
        ++index;
    }
    if (string[index] != '.') {
        throw std::invalid_argument("Bad date");
    }
    ++index;
    if (!isDigit(string[index])) {
        throw std::invalid_argument("Bad date");
    }
    while (isDigit(string[index])) {
        year = year * 10 + (string[index] - '0');
        ++index;
    }
    if (string[index] != '\0') {
        throw std::invalid_argument("Bad date");
    }
    if (!isValid(day, month, year)) {
        throw std::invalid_argument("Bad date string");
    }
    return Date(day, month, year);
}

std::string Date::str() const {
    std::string result;
    result.reserve(10);
    result.push_back('0' + day / 10);
    result.push_back('0' + day % 10);
    result.push_back('.');
    result.push_back('0' + month / 10);
    result.push_back('0' + month % 10);
    result.push_back('.');
    int y = year;
    result.push_back('0' + (y / 1000) % 10);
    result.push_back('0' + (y / 100) % 10);
    result.push_back('0' + (y / 10) % 10);
    result.push_back('0' + y % 10);
    return result;
}

int Date::getDay() const {
    return day;
}

int Date::getMonth() const {
    return month;
}

int Date::getYear() const {
    return year;
}
