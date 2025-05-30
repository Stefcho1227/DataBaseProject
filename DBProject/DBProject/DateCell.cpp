#include "DateCell.h"
DateCell::DateCell(const std::string& literal) : value() {
    if (literal == "NULL") {
        isNull = true;
    }
    if (!isNull) {
        value = Date::parse(literal.c_str());
    }
}

std::string DateCell::toString() const {
    return isNull ? "NULL" : value.str();
}

Cell* DateCell::clone() const{
    return new DateCell(*this);
}
