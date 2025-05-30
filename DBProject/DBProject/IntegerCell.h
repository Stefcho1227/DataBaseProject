#pragma once
#include "Cell.h"
#include "CellUtils.h"
class IntegerCell : public Cell {
private:
	int value;
	bool isValid(const std::string& input);
public:
	explicit IntegerCell(const std::string& literal);
	Cell* clone() const override;
	std::string toString() const override;
};