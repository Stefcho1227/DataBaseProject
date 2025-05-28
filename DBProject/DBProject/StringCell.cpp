#include "StringCell.h"

StringCell::StringCell(const std::string& literal) : Cell(literal){}

Cell* StringCell::clone() {
    return new StringCell(*this);
}
