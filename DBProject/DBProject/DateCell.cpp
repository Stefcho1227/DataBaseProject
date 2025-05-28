#include "DateCell.h"
//Тук ще видя защо parse не приема директно стринг 
DateCell::DateCell(const std::string& literal) : Cell(literal){
    if (!isNull) {
        value = Date::parse(literal.c_str());
    }
}

Cell* DateCell::clone() {
    return new DateCell(*this);
}
