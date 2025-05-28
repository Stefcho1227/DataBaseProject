#pragma once
#include <vector>
#include <string>
#include "CellUtils.h"
#include "Cell.h"
#include "IntegerCell.h"
#include "DoubleCell.h"
#include "StringCell.h"
#include "DateCell.h"


class Row {
private:
	std::vector<Cell*> cells;
	static Cell* makeCell(const std::string& literal, CellType expected);
public:
	Row(const std::vector<std::string>& literals, const std::vector<CellType>& types);
	void writeToStream(std::ostream& os) const;
	void print(const std::vector<int>& w) const;
	void setCell(size_t index, const std::string& literal, CellType expected);
	void addNullCell(CellType expected);
	size_t getSize() const;
	Cell* getCell(size_t index) const;
};