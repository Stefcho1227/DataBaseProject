#pragma once
#include <vector>
#include <string>
#include "iostream"
#include "Row.h"
#include "CellUtils.h"

class Table {
private:
	std::string name;
	std::vector<std::string> columnNames;
	std::vector<CellType> columnTypes;
	std::vector<Row> rows;
	void printHeader(const std::vector<int>& widths) const;
	bool containsCaseInsensitive(const std::string& cellVal, const std::string& searchVal) const;
public:
	Table(const std::string name, const std::vector<std::string>& colNames, const std::vector<CellType>& colTypes);
	std::vector<int> calculateColumnWidths() const;
	const std::string& getName() const;
	void print(int pageSize = -1) const;
	void exportTo(const std::string& fileName) const;
	void addColumn(const std::string& colName, CellType type);
	void insertRow(const std::vector<std::string>& literals);
	size_t deleteRows(size_t searchCol, const std::string& valueExact);
	size_t updateRows(size_t searchCol, const std::string& searchVal, size_t targetCol, const std::string& newVal);
	std::vector<Row> select(size_t col, const std::string& valueSubstr) const;
	void modifyColumn(size_t col, CellType newType, size_t& converted, size_t& failed);
	void writeToStream(std::ostream& os) const;
	void readFromStream(std::istream& is);
	const std::vector<std::string>& getColNames() const;
	const std::vector<CellType>& getColTypes() const;
	const std::vector<Row> getRows() const;
	void setRows(const std::vector<Row>& newRows);
};