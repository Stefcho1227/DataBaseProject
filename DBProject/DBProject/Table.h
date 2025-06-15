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
	void addColumn(const std::string& columnName, CellType type);
	void insertRow(const std::vector<std::string>& literals);
	void deleteRows(int searchColumn, const std::string& value);
	void updateRows(int searchColumn, const std::string& searchValue, int targetColumn, const std::string& newValue);
	std::vector<Row> select(int column, const std::string& value) const;
	void modifyColumn(int columnNumber, CellType newType, size_t& converted, size_t& failed);
	void writeToStream(std::ostream& os) const;
	void readFromStream(std::istream& is);
	const std::vector<std::string>& getColNames() const;
	const std::vector<CellType>& getColTypes() const;
	const std::vector<Row> getRows() const;
	void setRows(const std::vector<Row>& newRows);
};