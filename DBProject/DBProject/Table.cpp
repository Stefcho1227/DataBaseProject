#include "Table.h"

std::vector<int> Table::calculateColumnWidths() const {
    std::vector<int> w(columnNames.size(), 0);
    for (size_t i = 0; i < columnNames.size(); i++) {
        w[i] = columnNames[i].size();
    }
    for (size_t i = 0; i < rows.size(); i++) {
        for (size_t j = 0; j < rows[i].getSize(); j++) {
            w[j] = std::max(w[j], rows[i].getCell(j)->getStringLength());
        }
    }
    return w;
}

void Table::printHeader(const std::vector<int>& widths) const {
    for (size_t i = 0; i < columnNames.size(); i++) {
        if (i) {
            std::cout << "|";
        }
        std::cout.width(widths[i]);
        std::cout << columnNames[i];
    }
    std::cout << '\n';
    for (size_t i = 0; i < columnNames.size(); i++) {
        if (i) {
            std::cout << "+";
        }
        for (size_t j = 0; j < widths[i]; j++) {
            std::cout << "-";
        }
    }
    std::cout << '\n';
}

bool Table::containsCaseInsensitive(const std::string& cellVal, const std::string& searchVal) const{
    if (searchVal.empty()) {
        return true;
    }
    for (size_t i = 0; i + searchVal.size() <= cellVal.size(); i++) {
        bool correct = true;
        for (size_t j = 0; j < searchVal.size(); j++) {
            char a = cellVal[i + j];
            char b = searchVal[j];
            if (a >= 'A' && a <= 'Z') {
                a += 32;
            }
            if (b >= 'A' && b <= 'Z') {
                b += 32;
            }
            if (a != b) {
                correct = false;
                break;
            }
        }
        if (correct) {
            return true;
        }
    }
    return false;
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
    size_t per = (pageSize <= 0) ? rows.size() : pageSize;
    size_t page = 0;
    size_t total = rows.size();
    size_t lastPage = per == 0 ? 0 : (total - 1) / per;
    while (true) {
        printHeader(widths);
        size_t start = page * per;
        size_t end = std::min(start + per, total);
        for (size_t i = start; i < end; i++) {
            rows[i].print(widths);
        }
        if (per >= total) {
            break;
        }
        std::cout << "first, prev, next, last, quit\n";
        std::string input;
        std::cin >> input;
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
            std::cout << "please try again";
        }
    }
}

void Table::exportTo(const std::string& fileName) const {
    static std::string TYPES_STRINGS[(int)CellType::COUNT] = {"Integer", "Double", "Date", "String"};
    std::ofstream ofs(fileName);
    if (!ofs.is_open()) {
        throw std::invalid_argument("Error");
    }
    ofs << name;
    for (size_t i = 0; i < columnNames.size(); i++) {
        ofs << "|" << columnNames[i] << ":" << TYPES_STRINGS[(int)columnTypes[i]];
    }
    ofs << '\n';
    for (size_t i = 0; i < rows.size(); i++) {
        rows[i].writeToStream(ofs);
    }
}

void Table::addColumn(const std::string& colName, CellType type) {
    columnNames.push_back(colName);
    columnTypes.push_back(type);
    /*size_t index = columnTypes.size() - 1;*/
    for (size_t i = 0; i < rows.size(); i++) {
        //rows[i].setCell(index, "NULL", type);
        rows[i].addNullCell(/*"NULL", */type);
    }
}

void Table::insertRow(const std::vector<std::string>& literals) {
    if (literals.size() != columnTypes.size()) {
        throw std::invalid_argument("insert row has count mismatch");
    }
    rows.emplace_back(literals, columnTypes);
}

size_t Table::deleteRows(size_t searchCol, const std::string& valueExact) {
    if (searchCol >= columnTypes.size()) {
        throw std::invalid_argument("bad column");
    }
    size_t before = rows.size();
    if (before == 0) {
        return 0;
    }
    for (int i = rows.size() - 1; i >= 0 ; i--) {
        if (rows[i].getCell(searchCol)->raw() == valueExact) {
            rows.erase(rows.begin() + i);
        }
    }
    return before - rows.size();
}

size_t Table::updateRows(size_t searchCol, const std::string& searchVal, size_t targetCol, const std::string& newVal) {
    if (searchCol >= columnTypes.size() || targetCol >= columnTypes.size()) {
        throw std::invalid_argument("bad column");
    }
    bool isText = (columnTypes[searchCol] == CellType::String);
    size_t changes= 0;
    for (size_t i = 0; i < rows.size(); i++) {
        bool match = false;
        if (isText) {
            const std::string& cellVal = rows[i].getCell(searchCol)->raw();
            match = containsCaseInsensitive(cellVal, searchVal);
        }
        else {
            match = (rows[i].getCell(searchCol)->raw() == searchVal);
        }
        if (match) {
            rows[i].setCell(targetCol, newVal, columnTypes[targetCol]);
            changes++;
        }
    }
    return changes;
}

std::vector<Row> Table::select(size_t col, const std::string& valueSubstr) const {
    if (col >= columnTypes.size()) {
        throw std::invalid_argument("bad column");
    }
    std::vector<Row> result;
    bool isText = (columnTypes[col] == CellType::String);
    for (size_t i = 0; i < rows.size(); i++) {
        bool match = false;
        if (isText) {
            const std::string& cellVal = rows[i].getCell(col)->raw();
            match = containsCaseInsensitive(cellVal, valueSubstr);
        }
        else {
            match = (rows[i].getCell(col)->raw() == valueSubstr);
        }
        if (match) {
            result.push_back(rows[i]);
        }
    }
    return result;
}

void Table::modifyColumn(size_t col, CellType newType, size_t& converted, size_t& failed) {
    if (col >= columnTypes.size()) {
        throw std::invalid_argument("bad column");
    }
    columnTypes[col] = newType;
    converted = failed = 0;
    for (size_t i = 0; i < rows.size(); i++) {
        try {
            rows[i].setCell(col, rows[i].getCell(col)->raw(), newType);
            converted;;
        }
        catch (...) {
            rows[i].setCell(col, "NULL", newType);
            ++failed;
        }
    }
}

void Table::writeToStream(std::ostream& os) const {
    static const char* TYPES_STRINGS[(int)CellType::COUNT] = { "Integer","Double","Date","String"};
    os << "##" << name;
    for (size_t i = 0; i < columnNames.size(); i++) {
        os << columnNames[i] << ":" << TYPES_STRINGS[(int)columnTypes[i]];
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
        size_t position = 0;
        while (position < input.size()) {
            size_t comma = input.find(',', position);
            bool found = (comma < input.size());
            size_t length = found ? (comma - position) : (input.size() - position);
            std::string token = input.substr(position, length);
            while (!token.empty() && token[0] == ' ') {
                token.erase(token.begin());
            }
            literals.push_back(token);
            if (!found) {
                break;
            }
            position = comma + 2;
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
