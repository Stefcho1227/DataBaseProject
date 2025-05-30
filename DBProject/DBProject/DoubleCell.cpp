#include "DoubleCell.h"

bool DoubleCell::isValid(const std::string& literal) {
    if (literal.empty()) {
        return false;
    }
    size_t index = 0;
    if (literal[index] == '+' || literal[index] == '-') {
        index++;
    }
    if (index == literal.size()) {
        return false;
    }
    bool wasDot = false;
    bool leftDigit = false;
    bool rightDigit = false;
    for (; index < literal.size(); index++) {
        if (literal[index] == '.') {
            if (wasDot) {
                return false;
            }
            wasDot = true;
            continue;
        }
        if (!isDigit(literal[index])) {
            return false;
        }
        if (!wasDot) {
            leftDigit = true;
        }
        else {
            rightDigit = true;
        }
    }
    return wasDot && leftDigit && rightDigit;
}

DoubleCell::DoubleCell(const std::string& literal) : value(0.0) {
    if (literal == "NULL") {
        isNull = true;
    }
    if (!isNull) {
        if (!isValid(literal)) {
            throw std::invalid_argument("Invalid double: " + literal);
        }
        value = toDouble(literal);
    }
}

std::string DoubleCell::toString() const {
    return isNull ? "NULL" : std::to_string(value);
}

Cell* DoubleCell::clone() const {
    return new DoubleCell(*this);
}
