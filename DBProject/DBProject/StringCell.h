#pragma once
#include "Cell.h"
class StringCell : public Cell {
private:
	std::string value;
public:
	explicit StringCell(const std::string& literal);
	Cell* clone() const override;
	std::string toString() const;
};