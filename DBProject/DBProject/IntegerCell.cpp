#include "IntegerCell.h"

bool IntegerCell::isValid(const std::string& input) {
    if (input.empty()) {
        return false;
    }
    size_t index = (input[0] == '+' || input[0] == '-') ? 1 : 0;
    if (index == input.size()) {
        return false;
    }
    for (; index < input.size(); index++) {
        if (!isDigit(input[index])) {
            return false;
        }
    }
    return true;
}

IntegerCell::IntegerCell(const std::string& literal) : value(0) {
    if (literal == "NULL") {
        isNull = true;
    }
    if (!isNull) {
        if (!isValid(literal)) {
            throw std::invalid_argument("Invalid integer: " + literal);
        }
        value = toInt(literal);
    }
}
std::string IntegerCell::toString() const {
    return isNull ? "NULL" : std::to_string(value);
}

Cell* IntegerCell::clone() const {
    return new IntegerCell(*this);
}

