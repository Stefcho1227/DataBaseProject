#include "CommandHandler.h"

void CommandHandler::splitFirst(const std::string& line, std::string& command, std::string& args) const {
    std::string trimmed = trim(line);
    size_t sp = trimmed.find(' ');
    if (sp >= trimmed.size()) {
        command = trimmed;
        args = "";
    }
    else {
        command = trimmed.substr(0, sp);
        args = trim(trimmed.substr(sp + 1));
    }
}

std::vector<std::string> CommandHandler::splitArgs(const std::string& literals) const{
    std::vector<std::string> result;
    std::string current;
    bool isQuoted = false;
    for (size_t i = 0; i < literals.size(); i++) {
        char c = literals[i];
        if (c == '"' && (i == 0 || literals[i - 1] != '\\')) {
            isQuoted = !isQuoted;
            current.push_back(c);
        }
        else if (c == ',' && !isQuoted) {
            result.push_back(trim(current));
            current.clear();
        }
        else {
            current.push_back(c);
        }
    }
    result.push_back(trim(current));
    return result;
}

std::string CommandHandler::trim(const std::string& line) const{
    size_t left = 0;
    while (left < line.size() && line[left] == ' ') {
        ++left;
    }
    size_t right = line.size();
    while (right > left && line[right - 1] == ' ') {
        --right;
    }
    return line.substr(left, right - left);
}

std::vector<std::string> CommandHandler::splitArgsWithSpaces(const std::string& literals) {
    std::vector<std::string> result;
    std::string current;
    bool isQuoted = false;
    for (size_t i = 0; i < literals.size(); i++) {
        char c = literals[i];
        if (c == '"' && (i == 0 || literals[i - 1] != '\\')) {
            isQuoted = !isQuoted;
            current.push_back(c);
        }
        else if (c == ' ' && !isQuoted) {
            if (!current.empty()) {
                result.push_back(trim(current));
                current.clear();
            }
        }
        else {
            current.push_back(c);
        }
    }
    if (!current.empty()) {
        result.push_back(trim(current));
    }
    return result;
}

bool CommandHandler::execute(const std::string& command, const std::string& args) {
    if (command == "open") {
        db.open(args);
    }
    else if (command == "close") {
        db.close();
    }
    else if (command == "save") {
        db.save();
    }
    else if (command == "saveas") {
        db.saveAs(args);
    }
    else if (command == "showtables") {
        db.showTables();
    }
    else if (command == "describe") {
        db.describe(args);
    }
    else if (command == "print") {
        size_t sp = args.find(' ');
        std::string table;
        if (sp < args.size()) {
            table = args.substr(0, sp);
        }
        else {
            table = args;
        }
        int page = -1;
        if (sp < args.size()) {
            std::string pageStr = args.substr(sp + 1);
            if (!pageStr.empty()) {
                page = toInt(pageStr);
            }
        }
        db.print(table, page);
    }
    else if (command == "exit") {
        if (db.isOpen()) {
            db.close();
        }
        return false;
    }
    else if (command == "export") {
        size_t sp = args.find(' ');
        if (sp >= args.size()) {
            std::cout << "Input should be: export <table> <file>\n";
        }
        else {
            std::string table = args.substr(0, sp);
            std::string file = args.substr(sp + 1);
            db.exportTable(table, file);
        }
    }
    else if (command == "addcolumn") {
        std::vector<std::string> values = splitArgsWithSpaces(args);
        if (values.size() != 3) {
            std::cout << "Input should be: addcolumn <table> <column name> <column type>\n";
        }
        try {
            CellType type = db.stringToEnumType(values[2]);
            db.addColumn(values[0], values[1], type);
        }
        catch (const std::exception& ex) {
            std::cout << "Invalid column index: " << values[2] << '\n';
        }
    }
    else if (command == "insert") {
        size_t sp = args.find(' ');
        if (sp >= args.size()) {
            std::cout << "Input should be: insert <table> <values...> seperated with ,\n";
        }
        else {
            std::string table = args.substr(0, sp);
            std::string literals = args.substr(sp + 1);
            std::vector<std::string> values = splitArgs(literals);
            db.insert(table, values);
        }
    }
    else if (command == "delete") {
        std::vector<std::string> values = splitArgsWithSpaces(args);
        if (values.size() < 3) {
            std::cout << "Input should be: delete <table> <column> <value>\n";
        }
        int columnNumber = toInt(values[1]);
        try {
            db.deleteRows(values[0], columnNumber, values[2]);
        }
        catch (const std::exception& ex) {
            std::cout << "Invalid column index: " << columnNumber << '\n';
        }
    }
    else if (command == "select") {
        std::vector<std::string> values = splitArgsWithSpaces(args);
        if (values.size() < 3) {
            std::cout << "Input should be: select <column> <value> <table> [pages]\n";
        }
        int column = toInt(values[0]);
        int pages = -1;
        if (values.size() > 3) {
            pages = toInt(values[3]);
        }
        db.select(column, values[1], values[2], pages);
    }
    else if (command == "update") {
        std::vector<std::string> values = splitArgsWithSpaces(args);
        if (values.size() < 5) {
            std::cout << "Input should be: update <searchColumn> <searchValue> <targetColumn> <newValue>";
        }
        else {
            int searchColumn = toInt(values[1]);
            int targetColumn = toInt(values[3]);
            db.updateRows(values[0], searchColumn, values[2], targetColumn, values[4]);
        }
    }
    else if (command == "modify") {
        std::vector<std::string> values = splitArgsWithSpaces(args);
        if (values.size() != 3) {
            std::cout << "Input hsould be: modify <table> <column> <newType>";
        }
        int columnNumnber = toInt(values[1]);
        CellType newType = db.stringToEnumType(values[2]);
        db.modifyColumn(values[0], columnNumnber, newType);
    }
    else if (command == "help") {
        std::cout << "Commands:\n"
            << " open <file>                                           -> Open a database file\n"
            << " close                                                 -> Close currently open file\n"
            << " save                                                  -> Save the current file\n"
            << " saveas <file>                                         -> Save the current file into a new file\n"
            << " showtables                                            -> Show all tables\n"
            << " describe <table>                                      -> Show names and types of table\n"
            << " print <table> [pages]                                 -> Print rows of the table, pages is for interactive paging (first, prev, next, last, quit)\n"
            << " export <table> <file>                                 -> Export table to file\n"
            << " insert <table> <values>                               -> Insert new row into a table. Types of values should match the types of the columns\n"
            << " delete <table> <search column n> <search value>       -> Delete rows where column n contains the search value\n"

            << " update <table name> <search column n> \n"        
            "<search value> <target column n> <target value>        -> Update rows where search column n matches search value.Sets target column n to new value\n\n"
                                                
            << " select <column n> <value> <table> [pages]             -> Show rows where cloumn n matches value, pages is for interactive paging (first, prev, next, last, quit) \n"
            << " addcolumn <table> <name> <type>                       -> Add column to table with type\n"
            << " modify <table> <coumn n> <type>                       -> Modify column n in table to type\n"
            << " exit                                                  -> Exit the program\n";
    }
    else {
        std::cout << "Invalid command: " << command << '\n';
    }
    return true;
}

void CommandHandler::run() {
    std::string inputLine;
    while (std::cout << "db> ", std::getline(std::cin, inputLine)) {
        if (inputLine.empty()) continue;
        std::string command;
        std::string arguments;
        splitFirst(inputLine, command, arguments);
        try {
            if (!execute(command, arguments)) {
                break;
            }
        }
        catch (const std::exception& ex) {
            std::cout << "Error: " << ex.what() << '\n';
        }
    }
}
