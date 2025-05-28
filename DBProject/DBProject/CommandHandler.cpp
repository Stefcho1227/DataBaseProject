#include "CommandHandler.h"

void CommandHandler::splitFirst(const std::string& line, std::string& command, std::string& args) const {
    size_t start = 0;
    while (start < line.size() && line[start] == ' ') {
        ++start;
    }
    size_t end = start;
    while (end < line.size() && line[end] != ' ') {
        ++end;
    }
    command = line.substr(start, end - start);
    args = (end < line.size()) ? line.substr(end + 1) : "";
    while (!args.empty() && args[0] == ' ') {
        args.erase(args.begin());
    }
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
        std::string tbl = (sp < args.size()) ? args.substr(0, sp) : args;
        int page = -1;
        if (sp < args.size()) {
            std::string pageStr = args.substr(sp + 1);
            if (!pageStr.empty()) page = toInt(pageStr);
        }
        db.print(tbl, page);
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
            std::string tbl = args.substr(0, sp);
            std::string file = args.substr(sp + 1);
            db.exportTable(tbl, file);
        }
    }
    else if (command == "addcolumn") {
        std::string tbl;
        std::string colName;
        std::string typeStr;
        size_t p1 = args.find(' ');
        if (p1 >= args.size()) {
            std::cout << "Input should be: addcolumn <table> <column name> <column type>\n";
        }
        else {
            size_t p2 = args.find(' ', p1 + 1);
            if (p2 >= args.size()) {
                std::cout << "Input should be: addcolumn <table> <column name> <column type>\n";
            }
            else {
                tbl = args.substr(0, p1);
                colName = args.substr(p1 + 1, p2 - p1 - 1);
                typeStr = args.substr(p2 + 1);
                try {
                    CellType tp = db.stringToEnumType(typeStr);
                    db.addColumn(tbl, colName, tp);
                }
                catch (const std::exception& e) {
                    std::cout << "Invalid column type: " << typeStr << '\n';
                }
            }
        }
    }
    else if (command == "insert") {
        size_t sp = args.find(' ');
        if (sp >= args.size()) {
            std::cout << "Input should be: insert <table> <values...>\n";
        }
        else {
            std::string tbl = args.substr(0, sp);
            std::string rest = args.substr(sp + 1);
            std::vector<std::string> vals;
            std::string current;
            bool inQuotes = false;
            for (size_t i = 0; i <= rest.size(); ++i) {
                char c = (i < rest.size()) ? rest[i] : ',';
                if (c == '"' && (i == 0 || rest[i - 1] != '\\'))
                    inQuotes = !inQuotes, current.push_back(c);
                else if (c == ',' && !inQuotes) {
                    size_t a = 0, b = current.size();
                    while (a < b && current[a] == ' ') ++a;
                    while (b > a && current[b - 1] == ' ') --b;
                    vals.push_back(current.substr(a, b - a));
                    current.clear();
                    if (i + 1 < rest.size() && rest[i + 1] == ' ') ++i;
                }
                else {
                    current.push_back(c);
                }
            }
            db.insert(tbl, vals);
        }
    }
    else if (command == "delete") {
        std::string tbl;
        std::string colStr;
        std::string valStr;

        size_t p1 = args.find(' ');
        if (p1 >= args.size()) {
            std::cout << "Input should be: delete <table> <column> <value>\n";
        }
        else {
            size_t p2 = args.find(' ', p1 + 1);
            if (p2 >= args.size()) {
                std::cout << "Input should be: delete <table> <column> <value>\n";
            }
            else {
                tbl = args.substr(0, p1);
                colStr = args.substr(p1 + 1, p2 - p1 - 1);
                valStr = args.substr(p2 + 1);
                try {
                    size_t col = toInt(colStr);
                    db.deleteRows(tbl, col, valStr);
                }
                catch (const std::exception& ex) {
                    std::cout << "Invalid column index: " << colStr << '\n';
                }
            }
        }
    }
    else if (command == "select") {
        size_t p1 = args.find(' ');
        if (p1 >= args.size()) {
            std::cout << "Input should be: select <column> <value> <table> [pages]\n";
        }
        else {
            size_t p2 = args.find(' ', p1 + 1);
            if (p2 >= args.size()) {
                std::cout << "Input should be: select <column> <value> <table> [pages]\n";
            }
            else {
                size_t p3 = args.find(' ', p2 + 1);
                std::string colStr = args.substr(0, p1);
                std::string valStr = args.substr(p1 + 1, p2 - p1 - 1);
                std::string tblName;
                int pages = -1;

                if (p3 >= args.size()) {
                    tblName = args.substr(p2 + 1);
                }
                else {
                    tblName = args.substr(p2 + 1, p3 - p2 - 1);
                    std::string pagesStr = args.substr(p3 + 1);
                    pages = toInt(pagesStr);
                }

                size_t col = toInt(colStr);
                db.select(col, valStr, tblName, pages);
            }
        }
    }
    else if (command == "update") {
        size_t p1 = args.find(' ');
        if (p1 >= args.size()) {
            std::cout << "Input should be: update <table> <searchCol> <searchVal> <targetCol> <newVal>\n";
        }
        else {
            size_t p2 = args.find(' ', p1 + 1);
            if (p2 >= args.size()) {
                std::cout << "Input should be: update <table> <searchCol> <searchVal> <targetCol> <newVal>\n";
            }
            else {
                size_t p3 = args.find(' ', p2 + 1);
                if (p3 >= args.size()) {
                    std::cout << "Input should be: update <table> <searchCol> <searchVal> <targetCol> <newVal>\n";
                }
                else {
                    size_t p4 = args.find(' ', p3 + 1);
                    if (p4 >= args.size()) {
                        std::cout << "Input should be: update <table> <searchCol> <searchVal> <targetCol> <newVal>\n";
                    }
                    else {
                        std::string tbl = args.substr(0, p1);
                        std::string searchColStr = args.substr(p1 + 1, p2 - p1 - 1);
                        std::string searchVal = args.substr(p2 + 1, p3 - p2 - 1);
                        std::string targetColStr = args.substr(p3 + 1, p4 - p3 - 1);
                        std::string newVal = args.substr(p4 + 1);

                        size_t sCol = toInt(searchColStr);
                        size_t tCol = toInt(targetColStr);
                        db.updateRows(tbl, sCol, searchVal, tCol, newVal);
                    }
                }
            }
        }
    }
    else if (command == "modify") {
        size_t p1 = args.find(' ');
        if (p1 >= args.size()) {
            std::cout << "Input should be: modify <table> <column> <newType>\n";
        }
        else {
            size_t p2 = args.find(' ', p1 + 1);
            if (p2 >= args.size()) {
                std::cout << "Input should be: modify <table> <column> <newType>\n";
            }
            else {
                std::string tbl = args.substr(0, p1);
                std::string colStr = args.substr(p1 + 1, p2 - p1 - 1);
                std::string typeStr = args.substr(p2 + 1);

                size_t col = toInt(colStr);
                CellType nt = db.stringToEnumType(typeStr);
                db.modifyColumn(tbl, col, nt);
            }
        }
    }
    else {
        std::cout << "Невалидна команда: " << command << '\n';
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
            std::cout << "Грешка: " << ex.what() << '\n';
        }
    }
}
