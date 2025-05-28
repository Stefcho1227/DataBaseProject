#pragma once 
#include "Cell.h"
#include "Date.h"

class DateCell : public Cell {
private:
	Date value;
public:
	explicit DateCell(const std::string& literal);
	Cell* clone() override;
};