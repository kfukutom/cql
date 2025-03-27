// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <unordered_map>
#include <algorithm>
#include "Field.h"
using namespace std;

class Table {
public:
    friend class Database;
    std::string name;
    std::vector<ColumnType> colType;
    std::vector<std::string> colNames;
    std::vector<std::vector<Field>> insertCols;
}; // class Table


class Database {
private:
    bool quiet_mode;

public:
friend class Table;
    std::unordered_map<string, Table> db;
    bool enabled;
    uint32_t colNums;
    uint32_t rowNums;
    Database() : quiet_mode(false), enabled(false), colNums(0) {}

    // Main Parsers + Functions
    void getMode(int argc, char** argv);
    void help() const;
    bool isQuiet() const { return quiet_mode; }
    void run(); // main()
    bool validate(std::string input, const std::string funcName);

    // FOR MY DEBUGGING:
    void showDB() const;

    // SQL Commands, let ins be my col name;
    void sqlCreate();
    void sqlJoin(std::string ins);
    void sqlRemove();
    void sqlInsert();
    void sqlDelete();
    void sqlGenerate(std::string ins);

    // DB:
}; // class Database

#endif // DATABASE_HPP