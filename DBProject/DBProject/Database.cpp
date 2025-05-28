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
            bool found = (next < input.size());
            size_t tokenLenght = found ? (next - bar - 1) : (input.size() - bar - 1);
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

void DataBase::select(size_t col, const std::string& value, const std::string& tableName, int page) const {
    const Table* table = findTable(tableName);
    if (!table) {
        throw std::invalid_argument("No such table");
    }
    std::vector<Row> rows = table->select(col, value);
    if (rows.empty()) {
        std::cout << "no matches\n";
        return;
    }
    Table tmp("temp", table->getColNames(), table->getColTypes());
    tmp.setRows(rows);
    tmp.print(page);
}

void DataBase::addColumn(const std::string& table, const std::string& colName, CellType type) {
    Table* tableFound = findTable(table);
    if (!tableFound) {
        throw std::invalid_argument("No such table");
    }
    tableFound->addColumn(colName, type);
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

size_t DataBase::deleteRows(const std::string& table, size_t col, const std::string& value) {
    Table* tableFound = findTable(table);
    if (!tableFound) {
        throw std::invalid_argument("No such table");
    }
    unsavedChanges = true;
    return tableFound->deleteRows(col, value);
}

size_t DataBase::updateRows(const std::string& table, size_t searchCol, const std::string& searchValue, size_t targetCol, const std::string& newValue) {
    Table* tableFound = findTable(table);
    if (!tableFound) {
        throw std::invalid_argument("No such table");
    }
    unsavedChanges = true;
    return tableFound->updateRows(searchCol, searchValue, targetCol, newValue);
}

void DataBase::modifyColumn(const std::string& table, size_t col, CellType newType) {
    Table* tableFound = findTable(table);
    if (!tableFound) {
        throw std::invalid_argument("No such table");
    }
    size_t ok = 0;
    size_t bad = 0;
    tableFound->modifyColumn(col, newType, ok, bad);
    std::cout << "Converted: " << ok << ", failed: " << bad << '\n';
    unsavedChanges = true;
}

const std::vector<Table>& DataBase::getTables() const {
    return tables;
}
