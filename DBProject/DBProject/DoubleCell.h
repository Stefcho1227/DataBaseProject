#pragma once
#include "Cell.h"
#include "CellUtils.h"
class DoubleCell : public Cell {
private:
	double value;
	bool isValid(const std::string& literal);
public:
	explicit DoubleCell(const std::string& literal);
	Cell* clone() override;
};