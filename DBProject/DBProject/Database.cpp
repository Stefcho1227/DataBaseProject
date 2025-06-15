#include "Database.h"

void DataBase::readFromFile(const std::string& path) {

    tables.clear();
    filePath = path;
    unsavedChanges = false;
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::invalid_argument("Error");
    }
    std::string input;
    while (std::getline(in, input)) {
        if (input.empty()) {
            continue;
        }
        if (input.substr(0, 2) != "##") {
            throw std::invalid_argument("wrong db format");
        }
        size_t pos = 2;
        size_t bar = input.find('|', pos);
        std::string tableName = input.substr(pos, bar - pos);
        std::vector<std::string> columnNames;
        std::vector<CellType> columnTypes;
        while (bar < input.size()) {
            size_t next = input.find('|', bar + 1);
            bool found = false;
            if (next < input.size()) {
                found = true;
            }
            size_t tokenLenght = 0;
            if (found) {
                tokenLenght = next - bar - 1;
            }
            else {
                tokenLenght = input.size() - bar - 1;
            }
            std::string token = input.substr(bar + 1, tokenLenght);
            int colon = token.find(':');
            if (colon >= token.size()) {
                throw std::invalid_argument("wrong column");
            }
            std::string colName = token.substr(0, colon);
            std::string typeName = token.substr(colon + 1);
            columnNames.push_back(colName);
            columnTypes.push_back(stringToEnumType(typeName));
            if (!found) {
                break;
            }
            bar = next;
        }
        Table table(tableName, columnNames, columnTypes);
        table.readFromStream(in);
        tables.push_back(table);
    }
}

void DataBase::writeToFile(const std::string& path) const {
    std::ofstream out(path);
    if (!out.is_open()) {
        throw std::invalid_argument("Error");
    }
    for (size_t i = 0; i < tables.size(); i++) {
        tables[i].writeToStream(out);
    }
}

const Table* DataBase::findTable(const std::string& name) const {
    for (size_t i = 0; i < tables.size(); i++) {
        if (tables[i].getName() == name) {
            return &tables[i];
        }
    }
    return nullptr;
}

Table* DataBase::findTable(const std::string& name) {
    for (size_t i = 0; i < tables.size(); i++) {
        if (tables[i].getName() == name) {
            return &tables[i];
        }
    }
    return nullptr;
}

bool DataBase::isOpen() const {
    return !filePath.empty();
}

CellType DataBase::stringToEnumType(const std::string& s) {
    static const char* TYPES_STRINGS[(int)CellType::COUNT] = { "Integer","Double","Date","String"};
    for (size_t i = 0; i < (int)CellType::COUNT; i++) {
        if (s == TYPES_STRINGS[i]) {
            return (CellType)i;
        }
    }
    throw std::invalid_argument("Unknown type: " + s);
}

void DataBase::open(const std::string& path) {
    if (isOpen()) {
        close();
    }
    readFromFile(path);
}

void DataBase::close() {
    if (unsavedChanges) {
        writeToFile(filePath);
    }
    tables.clear();
    filePath.clear();
    unsavedChanges = false;
}


void DataBase::save() {
    if (!isOpen()) {
        throw std::invalid_argument("no db open");
    }
    writeToFile(filePath);
    unsavedChanges = false;
}

void DataBase::saveAs(const std::string& path) {
    if (!isOpen()) {
        throw std::invalid_argument("no db open");
    }
    writeToFile(path);
    filePath = path;
    unsavedChanges = false;
}

void DataBase::showTables() const {
    for (size_t i = 0; i < tables.size(); i++) {
        std::cout << tables[i].getName() << '\n';
    }
}

void DataBase::describe(const std::string& name) const {
    static const char* TYPES_STRINGS[(int)CellType::COUNT] = { "Integer","Double","Date","String"};
    for (size_t i = 0; i < tables.size(); i++) {
        if (tables[i].getName() == name) {
            for (size_t j = 0; j < tables[i].getColNames().size(); j++) {
                std::cout << j << ":" << tables[i].getColNames()[j] << " " << TYPES_STRINGS[(int)tables[i].getColTypes()[j]] << '\n';
            }
            return;
        }
    }
    std::cout << "Table with name: " << name << ". Not found!\n";
}

void DataBase::print(const std::string& name, int rpn) const{
    for (size_t i = 0; i < tables.size(); i++) {
        if (tables[i].getName() == name) {
            tables[i].print(rpn);
            return;
        }
    }
    std::cout << "Table with name: " + name << ". Not found!";
}

void DataBase::exportTable(const std::string& name, const std::string& path) const {
    const Table* table = findTable(name);
    if (!table) {
        throw std::invalid_argument("No such table");
    }
    table->exportTo(path);
}

void DataBase::select(int column, const std::string& value, const std::string& tableName, int page) const {
    const Table* table = findTable(tableName);
    if (!table) {
        throw std::invalid_argument("No such table");
    }
    std::vector<Row> rows = table->select(column, value);
    if (rows.empty()) {
        std::cout << "no matches\n";
        return;
    }
    Table temp("temp", table->getColNames(), table->getColTypes());
    temp.setRows(rows);
    temp.print(page);
}

void DataBase::addColumn(const std::string& table, const std::string& columnName, CellType type) {
    Table* tableFound = findTable(table);
    if (!tableFound) {
        throw std::invalid_argument("No such table");
    }
    tableFound->addColumn(columnName, type);
    unsavedChanges = true;
}

void DataBase::insert(const std::string& table, const std::vector<std::string>& literals) {
    Table* tableFound = findTable(table);
    if (!tableFound) {
        throw std::invalid_argument("No such table");
    }
    tableFound->insertRow(literals);
    unsavedChanges = true;
}

void DataBase::deleteRows(const std::string& table, int column, const std::string& value) {
    Table* tableFound = findTable(table);
    if (!tableFound) {
        throw std::invalid_argument("No such table");
    }
    unsavedChanges = true;
    tableFound->deleteRows(column, value);
}

void DataBase::updateRows(const std::string& table, int searchColumn, const std::string& searchValue, int targetColumn, const std::string& newValue) {
    Table* tableFound = findTable(table);
    if (!tableFound) {
        throw std::invalid_argument("No such table");
    }
    unsavedChanges = true;
    tableFound->updateRows(searchColumn, searchValue, targetColumn, newValue);
}

void DataBase::modifyColumn(const std::string& table, int columnNumber, CellType newType) {
    Table* tableFound = findTable(table);
    if (!tableFound) {
        throw std::invalid_argument("No such table");
    }
    size_t ok = 0;
    size_t bad = 0;
    tableFound->modifyColumn(columnNumber, newType, ok, bad);
    std::cout << "Converted: " << ok << ", failed: " << bad << '\n';
    unsavedChanges = true;
}

const std::vector<Table>& DataBase::getTables() const {
    return tables;
}
