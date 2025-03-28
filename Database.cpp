// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "Database.h"
#include <limits>
using namespace std;

void Database::getMode(int argc, char** argv) {
    int choice;
    int option_index = 0;
    struct option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"quiet", no_argument, nullptr, 'q'},
        {nullptr, 0, nullptr, '\0'}
    };
    
    while ((choice = getopt_long(argc, argv, "hq", long_options, &option_index)) != -1) {
        switch (choice) {
            case 'h':
                help();
                exit(0);
            case 'q':
                quiet_mode = true;
                break;
            default:
                std::cerr << "Unknown command line option" << std::endl;
                exit(1);
        }
    }
} // getMode()

void Database::help() const {
    std::cout << "Usage: ./database [OPTION]...\n"
              << "Options:\n"
              << "  -h, --help\t\tDisplay this help message\n"
              << "  -q, --quiet\t\tRun in quiet mode\n";
} // help()

// HELPERS:

void Database::showDB() const {
    if (db.empty()) {
        return;
    }

    for (const auto& [tableName, table] : db) {
        std::cout << "Table: " << tableName << "\n";
        std::cout << "  Columns: ";
        for (const auto& col : table.colNames) {
            std::cout << col << " ";
        }
        std::cout << "\n";

        if (table.insertCols.empty()) {
            std::cout << "  No rows inserted.\n";
        } else {
            for (const auto& row : table.insertCols) {
                std::cout << "  ";
                for (const auto& field : row) {
                    std::cout << field << " ";
                }
                std::cout << "\n";
            }
        }
    }
} // showDB()

bool Database::validate(std::string input, const std::string funcName) const {
    std::string junk;
    if (funcName == "CREATE") {
        if (db.find(input) != db.end()) {
            std::cout << "Error during CREATE: Cannot create already existing table " << input << "\n";
            std::getline(std::cin, junk);
            return false;
        }
    } else if (funcName == "REMOVE") {
        if (db.find(input) == db.end()) {
            std::cout << "Error during REMOVE: Cannot remove " << input << "\n";
            std::getline(std::cin, junk);
            return false;
        }
    } else if (db.find(input) == db.end()) {
        std::cout << "Error during " << funcName << ": " << input << " does not name a table in the database\n";
        return false;
    }
    return true;
} // validate()

void Database::run() {
    std::string command;
    std::string junk;

    while (true) {
        std::cout << "% ";
        std::cin >> command;
        char firstChar = command[0];

        switch (firstChar) {
            case '#': {
                std::getline(std::cin, junk);
                break;
            }
            case 'C': {
                sqlCreate();
                break;
            }
            case 'R': {
                sqlRemove();
                break;
            }
            case 'I': {
                sqlInsert();
                break;
            }
            case 'Q': {
                if (command == "QUIT") {
                    std::cout << "Thanks for being silly! \n";
                    return;
                }
                break;
            }
            case 'P': {
                sqlPrint();
                break;
            }
            case 'S': {
                showDB();
                break;
            }
            default: {
                std::cout << "Error: unrecognized command\n";
                std::getline(std::cin, junk);
                break;
            }
        }
    }
} // run()

void Database::sqlCreate() {
    std::string table;
    std::uint32_t colNums;
    std::cin >> table >> colNums;

    // if (validate(table, "CREATE"))
    if (!validate(table, "CREATE")) {
        return; // fixed here today
    }

    db.emplace(table, Table());
    db[table].colType.resize(colNums);
    db[table].colNames.resize(colNums);

    for (std::uint32_t i = 0; i < colNums; ++i) {
        std::string type;
        std::cin >> type;
        if (type == "string") {
            db[table].colType[i] = ColumnType::String;
        } 
        else if (type == "int") {
            db[table].colType[i] = ColumnType::Int;
        } 
        else if (type == "bool") {
            db[table].colType[i] = ColumnType::Bool;
        } 
        else if (type == "double") {
            db[table].colType[i] = ColumnType::Double;
        } 
        else {
            std::cerr << "Unknown column type: " << type << "\n";
            std::string junk;
            std::getline(std::cin, junk);
            db.erase(table);
            return;
        }
    }

    for (std::uint32_t i = 0; i < colNums; ++i) {
        std::cin >> db[table].colNames[i];
    }

    std::cout << "New table " << table << " with column(s): ";
    for (const auto &name : db[table].colNames) {
        std::cout << name << " ";
    }
    std::cout << "created" << std::endl;
} // sqlCreate()

void Database::sqlRemove() {
    string target;
    cin >> target;
    if (!validate(target, "REMOVE")) {
        return;
    }
    db.erase(db.find(target));
    std::cout << "Table " << target << " deleted\n";
} // sqlRemove()

// ------------------------------------ TABLE COMMANDS ------------------------------------

void Database::sqlInsert() {
    std::string into, table, rows;
    uint32_t numRows;

    std::cin >> into >> table >> numRows >> rows;
    if (!validate(table, "INSERT")) {
        return;
    }

    rowNums += numRows;
    Table &t = db[table];
    uint32_t numCols = static_cast<uint32_t>(t.colType.size());
    uint32_t startIndex = static_cast<uint32_t>(t.insertCols.size());

    for (uint32_t i = 0; i < numRows; ++i) {
        std::vector<Field> row;
        for (uint32_t j = 0; j < numCols; ++j) {
            if (t.colType[j] == ColumnType::Int) {
                int val;
                std::cin >> val;
                row.emplace_back(val);
            } else if (t.colType[j] == ColumnType::Double) {
                double val;
                std::cin >> val;
                row.emplace_back(val);
            } else if (t.colType[j] == ColumnType::Bool) {
                std::string val;
                std::cin >> val;
                row.emplace_back(val == "true");
            } else if (t.colType[j] == ColumnType::String) {
                std::string val;
                std::cin >> val;
                row.emplace_back(val);
            }
        }
        t.insertCols.push_back(row);
    }

    uint32_t endIndex = static_cast<uint32_t>(t.insertCols.size()) - 1;
    std::cout << "Added " << numRows << " rows to " << table
              << " from position " << startIndex << " to " << endIndex << std::endl;
} // sqlInsert()


// Printing


bool Database::validateTable(Table& ins, std::vector<std::string> cols) const {
    if (ins.colNames.empty()) {
        std::cout << "Empty table LOL." << std::endl;
        return false;
    } else {
        for (const auto &col : cols) {
            if (find(ins.colNames.begin(), ins.colNames.end(), col) == ins.colNames.end()) {
                cout << "Error during PRINT: " << col << " does not name a column in table" << endl;
                return false;
            }
        }
    }
    return true;
} // validateTable() const

void Database::sqlPrint() {
    // print func:
    string token;
    std::cin >> token;
    if (token != "FROM") {
        std::cerr << "Syntax error: expected FROM" << std::endl;
        return;
    } //if

    string table;
    uint32_t N;
    std::cin >> table;
    // helper here,
    validate(table, "PRINT");
    std::cin >> N;
    std::vector<std::string> targets(N);
    for (std::uint32_t i = 0; i < N; ++i) {
        std::cin >> targets[i];
    }

    //column checking.
    Table &t = db[table];
    if (!validateTable(t, targets)) {
        return;
    }
    //cout << "Passed.\n";

    string junk;
    bool stats = false;
    std::cin >> junk; // WHERE or ALL
    if (junk == "ALL") {
        printAll();
        return;
    }
    if (junk == "WHERE") {
        printWhere();
        return;
    }
    else {
        std::cerr << "No expected clause in input, try ALL or WHERE.\n";
        return; 
    }
} // sqlPrint()