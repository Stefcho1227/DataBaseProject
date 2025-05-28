#pragma once
#include <vector>
#include <string>
#include <fstream>
#include "Table.h"
class DataBase {
private:
	std::string filePath;
	std::vector<Table> tables;
	bool unsavedChanges;
	void readFromFile(const std::string& path);
	void writeToFile(const std::string& path) const;
	const Table* findTable(const std::string& name) const;
	Table* findTable(const std::string& name);
public:
	CellType stringToEnumType(const std::string& s);
	bool isOpen() const;
	void open(const std::string& path);
	void close();
	void save();
	void saveAs(const std::string& path);
	void showTables() const;
	void describe(const std::string& name) const;
	void print(const std::string& name, int rpn = -1) const;
	void exportTable(const std::string& name, const std::string& path) const;
	void select(size_t col, const std::string& value, const std::string& tableName, int page = -1) const;
	void addColumn(const std::string& table, const std::string& colName, CellType type);
	void insert(const std::string& table, const std::vector<std::string>& literals);
	size_t deleteRows(const std::string& table, size_t col, const std::string& value);
	size_t updateRows(const std::string& table, size_t searchCol, const std::string& searchValue, size_t targetCol, const std::string& newValue);
	void modifyColumn(const std::string& table, size_t col, CellType newType);
	const std::vector<Table>& getTables() const;
};