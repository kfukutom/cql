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

    // bst + hash
    bool has_hash, has_bst;
    uint32_t bst_index;
    uint32_t hash_index;
    std::map<Field, vector<uint32_t>> bst;
    std::unordered_map<Field, vector<uint32_t>> hash; 
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

    // helpers
    bool validate(std::string input, const std::string funcName) const;
    bool validateTable(Table& ins, std::vector<std::string> cols) const;

    // FOR MY DEBUGGING:
    void showDB() const;

    // SQL Commands, let ins be my col name;
    void sqlCreate();
    void sqlJoin();
    void sqlRemove();
    void sqlInsert();
    void sqlDelete();
    void sqlGenerate();

    // Various Prints LOL
    void sqlPrint();
    void printAll(Table& t, const string tableName, vector<string>& targets);
    void printWhere(Table& t, const string& tableName, const vector<string>& targets); // might need params additional!

    // DB MemAlloc:
    void cleanup();
}; // class Database

#endif // DATABASE_HPP