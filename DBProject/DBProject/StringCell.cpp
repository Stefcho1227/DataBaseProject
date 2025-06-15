#include "StringCell.h"



StringCell::StringCell(const std::string& literal) : value("") {
    if (literal == "NULL") {
        isNull = true;
    }
    if (!isNull) {
        value = literal;
    }
}

std::string StringCell::toString() const {
    return isNull ? "NULL" : value;
}

Cell* StringCell::clone() const {
    return new StringCell(*this);
}
