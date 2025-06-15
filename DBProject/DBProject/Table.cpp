#include "Table.h"

std::vector<int> Table::calculateColumnWidths() const {
    std::vector<int> widths(columnNames.size(), 0);
    for (size_t i = 0; i < columnNames.size(); i++) {
        widths[i] = columnNames[i].size();
    }
    for (size_t i = 0; i < rows.size(); i++) {
        for (size_t j = 0; j < rows[i].size(); j++) {
            widths[j] = std::max(widths[j], rows[i].getCell(j)->getStringLength());
        }
    }
    return widths;
}

void Table::printHeader(const std::vector<int>& widths) const {
    for (size_t i = 0; i < columnNames.size(); i++) {
        if (i > 0) {
            std::cout << "|";
        }
        std::cout.width(widths[i]);
        std::cout << columnNames[i];
    }
    std::cout << '\n';
    for (size_t i = 0; i < columnNames.size(); i++) {
        if (i > 0) {
            std::cout << "+";
        }
        for (size_t j = 0; j < widths[i]; j++) {
            std::cout << "-";
        }
    }
    std::cout << '\n';
}

bool Table::containsCaseInsensitive(const std::string& cellString, const std::string& userString) const {
    std::string value = unescape(stripQuotes(cellString));
    std::string input = unescape(stripQuotes(userString));
    for (size_t i = 0; i < value.size(); i++) {
        value[i] = lowerChar(value[i]);
    } 
    for (size_t i = 0; i < input.size(); i++) {
        input[i] = lowerChar(input[i]);
    }
    if (input.empty()) return true;
    return value.find(input) < value.size();
}

Table::Table(const std::string name, const std::vector<std::string>& colNames, const std::vector<CellType>& colTypes)
    : name(name), columnNames(colNames), columnTypes(colTypes){
    if (colNames.empty() || colNames.size() != colTypes.size()) {
        throw std::invalid_argument("Table: invalid schema");
    }

}

const std::string& Table::getName() const {
    return name;
}

void Table::print(int pageSize) const {
    if (columnNames.empty()) {
        return;
    }
    std::vector<int> widths = calculateColumnWidths();
    size_t pageLimit = 0;
    if (pageSize <= 0) {
        pageLimit = rows.size();
    }
    else {
        pageLimit = pageSize;
    }
    size_t page = 0;
    size_t total = rows.size();
    size_t lastPage = 0;
    if (pageLimit != 0) {
        lastPage = (total - 1) / pageLimit;
    }
    bool shouldPrintAgain = true;
    while (true) {
        if (shouldPrintAgain) {
            printHeader(widths);
            size_t start = page * pageLimit;
            size_t end = std::min(start + pageLimit, total);
            for (size_t i = start; i < end; i++) {
                rows[i].print(widths);
            }
        }
        if (pageLimit >= total) {
            break;
        }
        std::cout << "first, prev, next, last, quit\n";
        std::string input;
        std::cin >> input;
        shouldPrintAgain = true;
        if (input == "next" && page < lastPage) {
            page++;
        }
        else if (input == "prev" && page > 0) {
            page--;
        }
        else if (input == "first") {
            page = 0;
        }
        else if (input == "last") {
            page = lastPage;
        }
        else if (input == "quit") {
            break;
        }
        else {
            std::cout << "please try again\n";
            shouldPrintAgain = false;
        }
    }
}

void Table::exportTo(const std::string& fileName) const {
    static std::string TYPES_STRINGS[(int)CellType::COUNT] = {"Integer", "Double", "Date", "String"};
    std::ofstream ofs(fileName);
    if (!ofs.is_open()) {
        throw std::invalid_argument("Error");
    }
    ofs << "##" << name;
    for (size_t i = 0; i < columnNames.size(); i++) {
        ofs << "|" << columnNames[i] << ":" << TYPES_STRINGS[(int)columnTypes[i]];
    }
    ofs << '\n';
    for (size_t i = 0; i < rows.size(); i++) {
        rows[i].writeToStream(ofs);
    }
}

void Table::addColumn(const std::string& columnName, CellType type) {
    columnNames.push_back(columnName);
    columnTypes.push_back(type);
    for (size_t i = 0; i < rows.size(); i++) {
        rows[i].addNullCell(type);
    }
}

void Table::insertRow(const std::vector<std::string>& literals) {
    if (literals.size() != columnTypes.size()) {
        throw std::invalid_argument("insert row has count mismatch");
    }
    rows.emplace_back(literals, columnTypes);
}

void Table::deleteRows(int searchColumn, const std::string& value) {
    if (searchColumn >= columnTypes.size()) {
        throw std::invalid_argument("bad column");
    }
    for (int i = rows.size() - 1; i >= 0 ; i--) {
        if (rows[i].getCell(searchColumn)->toString() == value) {
            rows.erase(rows.begin() + i);
        }
    }
}

void Table::updateRows(int searchColumn, const std::string& searchValue, int targetColumn, const std::string& newValue) {
    if (searchColumn >= columnTypes.size() || targetColumn >= columnTypes.size()) {
        throw std::invalid_argument("bad column");
    }
    bool isText = (columnTypes[searchColumn] == CellType::String);
    for (size_t i = 0; i < rows.size(); i++) {
        bool match = false;
        if (isText) {
            match = containsCaseInsensitive(rows[i].getCell(searchColumn)->toString(), searchValue);
        }
        else {
            match = (rows[i].getCell(searchColumn)->toString() == searchValue);
        }
        if (match) {
            rows[i].setCell(targetColumn, newValue, columnTypes[targetColumn]);
        }
    }
}

std::vector<Row> Table::select(int column, const std::string& value) const {
    if (column >= columnTypes.size()) {
        throw std::invalid_argument("bad column");
    }
    std::vector<Row> result;
    bool isText = (columnTypes[column] == CellType::String);
    for (size_t i = 0; i < rows.size(); i++) {
        bool match = false;
        if (isText) {
            match = containsCaseInsensitive(rows[i].getCell(column)->toString(), value);
        }
        else {
            match = (rows[i].getCell(column)->toString() == value);
        }
        if (match) {
            result.push_back(rows[i]);
        }
    }
    return result;
}

void Table::modifyColumn(int columnNumber, CellType newType, size_t& converted, size_t& failed) {
    if (columnNumber >= columnTypes.size()) {
        throw std::invalid_argument("bad column");
    }
    columnTypes[columnNumber] = newType;
    for (size_t i = 0; i < rows.size(); i++) {
        try {
            rows[i].setCell(columnNumber, rows[i].getCell(columnNumber)->toString(), newType);
            ++converted;
        }
        catch (...) {
            rows[i].setCell(columnNumber, "NULL", newType);
            ++failed;
        }
    }
}

void Table::writeToStream(std::ostream& os) const {
    static const char* TYPES_STRINGS[(int)CellType::COUNT] = { "Integer","Double","Date","String"};
    os << "##" << name;
    for (size_t i = 0; i < columnNames.size(); i++) {
        os << "|" << columnNames[i] << ":" << TYPES_STRINGS[(int)columnTypes[i]];
    }
    os << '\n';
    for (size_t i = 0; i < rows.size(); i++) {
        rows[i].writeToStream(os);
    }
    os << '\n';
}

void Table::readFromStream(std::istream& is) {
    rows.clear();
    std::string input;
    while (std::getline(is, input)) {
        if (input.empty() || input[0] == '#') {
            break;
        }
        std::vector<std::string> literals;
        std::string current;
        bool isQuoted = false;
        for (size_t i = 0; i < input.size(); i++) {
            if (input[i] == '"' && (i == 0 || input[i - 1] != '\\')) {
                isQuoted = !isQuoted;
                current.push_back(input[i]);
            }
            else if (input[i] == ',' && !isQuoted) {
                literals.push_back(current);
                current.clear();
                if (i + 1 < input.size() && input[i + 1] == ' ') {
                    ++i;
                }
            }
            else {
                current.push_back(input[i]);
            }
        }
        if (!current.empty()) {
            literals.push_back(current);
        }
        rows.emplace_back(literals, columnTypes);
    }
}

const std::vector<std::string>& Table::getColNames() const {
    return columnNames;
}

const std::vector<CellType>& Table::getColTypes() const {
    return columnTypes;
}

const std::vector<Row> Table::getRows() const {
    return rows;
}

void Table::setRows(const std::vector<Row>& newRows) {
    rows = newRows;
}
