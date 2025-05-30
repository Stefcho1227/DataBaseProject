#include "Cell.h"

Cell::Cell(bool isNull) : isNull(isNull) {}

void Cell::writeToStream(std::ostream& os) const {
    os << toString();
}

void Cell::print(int width) const {
    std::cout.width(width);
    std::cout << toString();
}

int Cell::getStringLength() const {
    return static_cast<int>(toString().length());
}

bool Cell::isNullValue() const{
    return isNull;
}

