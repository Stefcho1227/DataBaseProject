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
	Row(const Row& other);
	Row& operator=(const Row& other);
	~Row();
	void writeToStream(std::ostream& os) const;
	void print(const std::vector<int>& columnWidths) const;
	void addNullCell(CellType expected);
	Cell* getCell(int index) const;
	void setCell(int index, const std::string& literal, CellType expected);
	size_t size() const;
};