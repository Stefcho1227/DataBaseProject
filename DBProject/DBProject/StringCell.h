#pragma once
#include "Cell.h"
class StringCell : public Cell {
public:
	explicit StringCell(const std::string& literal);
	Cell* clone() override;
};