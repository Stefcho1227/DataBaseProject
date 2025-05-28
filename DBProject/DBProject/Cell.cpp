#include "Cell.h"

Cell::Cell(const std::string& initial) : isNull(initial == "NULL"), text(initial) {}

void Cell::writeToStream(std::ostream& os) const {
    os << text;
}

void Cell::print(int width) const {
    std::cout.width(width);
    std::cout << text;
}

int Cell::getStringLength() const {
    return static_cast<int>(text.length());
}

const std::string& Cell::raw() const {
    return text;
}
