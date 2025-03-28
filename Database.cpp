// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "Database.h"
#include "Comparators.h"
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
                std::cerr << "Unknown command line option\n";
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
        if (!table.bst.empty()) {
            for (const auto& [key, indices] : table.bst) {
                for (uint32_t idx : indices) {
                    cout << idx << ' ';
                }
            }
            cout << table.bst_index << '\n';
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
            case 'P': {
                sqlPrint();
                break;
            }
            case 'S': {
                showDB();
                break;
            }
            case 'J': { // join
                std::cout << "JOIN is not implemented yet.\n";
                break;
            }
            case 'D': {
                sqlDelete();
                break;
            }
            case 'G': {
                sqlGenerate();
                break;
            }
            case 'Q': {
                if (command == "QUIT") {
                    std::cout << "Thanks for being silly! \n";
                    return;
                }
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
            db.erase(table);
            std::cerr << "Unknown column type: " << type << "\n";
            string junk;
            std::getline(cin, junk);
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
    std::cout << "created\n";
} // sqlCreate()

void Database::sqlRemove() {
    string target;
    string junk;
    cin >> target;
    if (!validate(target, "REMOVE")) {
        getline(cin, junk);
        return;
    }
    db.erase(db.find(target));
    std::cout << "Table " << target << " removed\n";
} // sqlRemove()

// ------------------------------------ TABLE COMMANDS ------------------------------------

void Database::sqlInsert() {
    string into, table, rows;
    string junk;
    uint32_t numRows;

    std::cin >> into >> table >> numRows >> rows;
    if (!validate(table, "INSERT")) {
        getline(cin, junk);
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

    if (!t.bst.empty()) {
        t.bst.clear();
        for (uint32_t j = 0; j < t.insertCols.size(); j++) {
            t.bst[t.insertCols[j][t.bst_index]].push_back(j);
        } // for()
    }

    uint32_t endIndex = static_cast<uint32_t>(t.insertCols.size()) - 1;
    std::cout << "Added " << numRows << " rows to " << table
              << " from position " << startIndex << " to " << endIndex << '\n';
} // sqlInsert()


// ------------------------------------ PRINT / VALIDATING TB COMMANDS ------------------------------------


bool Database::validateTable(Table& ins, std::vector<std::string> cols) const {
    if (ins.colNames.empty()) {
        std::cout << "Empty table LOL.\n";
        return false;
    } else {
        for (const auto &col : cols) {
            if (find(ins.colNames.begin(), ins.colNames.end(), col) == ins.colNames.end()) {
                cout << "Error during PRINT: " << col << " does not name a column in table\n";
                return false;
            }
        }
    }
    return true;
} // validateTable() const

void Database::sqlPrint() {
    string token;
    std::cin >> token;
    if (token != "FROM") {
        std::cerr << "Syntax error: expected FROM\n";
        return;
    }

    string table;
    uint32_t N;
    std::cin >> table;
    if (!validate(table, "PRINT")) {
        string trash;
        std::getline(std::cin, trash);
        return;
    }

    std::cin >> N;
    std::vector<std::string> targets(N);
    for (std::uint32_t i = 0; i < N; ++i) {
        std::cin >> targets[i];
    }

    Table &t = db[table];
    if (!validateTable(t, targets)) {
        string trash;
        std::getline(std::cin, trash);
        return;
    }

    string junk;
    std::cin >> junk;
    if (junk == "ALL") {
        printAll(t, table, targets);
        return;
    }
    if (junk == "WHERE") {
        printWhere(t, table, targets);
        return;
    } else {
        std::cerr << "No expected clause in input, try ALL or WHERE.\n";
        return;
    }
} // sqlPrint()

void Database::printAll(Table& t, std::string tableName, std::vector<std::string>& targets) {

    vector<uint32_t> indices;
    for (const auto &target : targets) {
        auto it = find(t.colNames.begin(), t.colNames.end(), target);
        if (it != t.colNames.end()) {
            indices.push_back(static_cast<uint32_t>(it - t.colNames.begin()));
        }
    }

    if (!quiet_mode) {
        for (const auto &idx : indices) {
            cout << t.colNames[idx] << " ";
        }
        cout << '\n';

        for (const auto &row : t.insertCols) {
            for (const auto &idx : indices) {
                cout << row[idx] << " ";
            }
            cout << '\n';
        }
    }

    cout << "Printed " << t.insertCols.size() << 
        " matching rows from " << tableName << '\n';
} // printAll()

void Database::printWhere(Table& t, const string& tableName, const vector<string>& targets) {
    string colName;
    char op;
    string val;
    cin >> colName >> op >> val;

    // Validate WHERE column
    auto it = find(t.colNames.begin(), t.colNames.end(), colName);
    if (it == t.colNames.end()) {
        cout << "Error during PRINT: " << colName << " does not name a column in " << tableName << '\n';
        string junk;
        getline(cin, junk);
        return;
    }
    std::cout << targets.at(0);
} // printWhere()

void Database::sqlGenerate() {
    string junk, tableName, indexType, col;
    cin >> junk;
    if (junk != "FOR") {
        cout << "Syntax error: expected FOR" << '\n';
        return;
    }

    cin >> tableName;
    cin >> indexType;
    cin >> junk;
    cin >> junk;
    cin >> col;

    if (db.find(tableName) == db.end()) {
        cout << "Error during GENERATE: " << tableName << " does not name a table in the database\n";
        return;
    }

    Table &t = db[tableName];
    if (find(t.colNames.begin(), t.colNames.end(), col) == t.colNames.end()) {
        cout << "Error during GENERATE: " << col << " does not name a column in " << tableName << '\n';
        return;
    }

    t.bst.clear();
    t.hash.clear();

    uint32_t col_index = 0;
    for (uint32_t i = 0; i < t.colNames.size(); i++) {
        if (t.colNames[i] == col) {
            col_index = i;
            break;
        }
    }
    t.bst_index = col_index;

    for (uint32_t j = 0; j < t.insertCols.size(); j++) {
        Field f = t.insertCols[j][col_index];
        if (indexType == "hash")
            t.hash[f].push_back(j);
        else if (indexType == "bst")
            t.bst[f].push_back(j);
        else {
            cout << "Error during GENERATE: " << indexType << " is not a valid index type" << '\n';
            return;
        }
    }

    if (indexType == "hash")
        cout << "Generated hash index for table " << tableName << " on column "
             << col << ", with " << t.hash.size() << " distinct keys\n";
    else
        cout << "Generated bst index for table " << tableName << " on column "
             << col << ", with " << t.bst.size() << " distinct keys\n";
} // sqlGenerate()
 

void Database::sqlJoin() {
    // IMPLEMENTATION HERE:
} // sqlJoin()


void Database::sqlDelete() {
    string token;
    string trash; // junk collection
    cin >> token;
    if (token != "FROM") {
        getline(cin, trash);
        cout << "Syntax error: expected FROM" << endl;
        return;
    }
    string tableName;
    cin >> tableName;
    if (!validate(tableName, "DELETE")) {
        getline(cin, trash);
        return;
    }
    
    string whereToken;
    cin >> whereToken;
    if (whereToken != "WHERE") {
        getline(cin, trash);
        cout << "Syntax error: expected WHERE" << endl;
        return;
    }
    string colname;
    cin >> colname;
    Table &T = db[tableName];
    if (find(T.colNames.begin(), T.colNames.end(), colname) == T.colNames.end()) {
        cout << "Error during DELETE: " << colname << " does not name a column in " << tableName << endl;
        string trash;
        getline(cin, trash);
        return;
    }
    uint32_t colIndex = 0;
    while (T.colNames[colIndex] != colname) {
        colIndex++;
    }
    char op;
    string value;
    cin >> op;
    cin >> value;
    Field entry(value);

    // prior size is saved + comparators from Comparators.h
    // refer to COMPARATORS.H
    uint32_t before = static_cast<uint32_t>(T.insertCols.size());
    switch (op) {
    case '>':
        T.insertCols.erase(remove_if(
            T.insertCols.begin(), T.insertCols.end(),
            compare_greater(colIndex, entry)), T.insertCols.end());
        break;
    case '<':
        T.insertCols.erase(remove_if(
            T.insertCols.begin(), T.insertCols.end(),
            compare_less(colIndex, entry)), T.insertCols.end());
        break;
    case '=':
        T.insertCols.erase(remove_if(
            T.insertCols.begin(), T.insertCols.end(),
            compare_equal(colIndex, entry)), T.insertCols.end());
        break;
    default:
        string garb;
        getline(cin, garb);
        //cout << "Invalid Syntax\n";
        return;
    }

    if (!T.bst.empty()) {
        T.bst.clear();
        for (uint32_t j = 0; j < T.insertCols.size(); j++) {
            T.bst[T.insertCols[j][T.bst_index]].push_back(j);
        } // for()
    }
    cout << "Deleted " << (before - T.insertCols.size()) << " rows from " << tableName << endl;
} //sqlDelete(), complete no more memory leaks.

// ------------------------------------ DATABASE MISC. ------------------------------------

void Database::cleanup() {
    string tb_name;
    cin >> tb_name;
    string junk;
    getline(cin, junk);

    auto it = db.find(tb_name);
    if (it == db.end()) {
        cout << "Error: Table " << tb_name << " does not exist\n";
        return;
    } //if()
    it->second.colNames.clear();
    it->second.colType.clear();
    it->second.insertCols.clear();
} // cleanup()