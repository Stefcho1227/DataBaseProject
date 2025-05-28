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

DoubleCell::DoubleCell(const std::string& literal) : Cell(literal) {
    if (isNull) {
        return;
    }
    if (!isValid(literal)) {
        throw std::invalid_argument("Invalid double: " + literal);
    }
    value = toDouble(literal);
}

Cell* DoubleCell::clone() {
    return new DoubleCell(*this);
}
