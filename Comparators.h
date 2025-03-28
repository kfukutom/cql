// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include <vector>
#include "Field.h"
using namespace std;

#ifndef COMPARATORS_HPP
#define COMPARATORS_HPP

/*
This file is for the BST implementation of my code in 
main.cpp + Database.cpp / Database.h implementation.
These are functors / comparators for the BST std::map<>
*/

struct compare_greater {
    uint32_t idx;
    Field entry;
    compare_greater(uint32_t i,const Field& e) : idx(i), entry(e) {}

    bool operator()(const vector<Field>& row) const {
        return row[idx] > entry;
    }
};

struct compare_less {
    uint32_t idx;
    Field entry;
    compare_less(uint32_t i, const Field& e) : idx(i), entry(e) {}

    bool operator()(const vector<Field>& row) const {
        return row[idx] < entry;
    }
};

struct compare_equal {
    uint32_t idx;
    Field entry;
    compare_equal(uint32_t i, const Field& e) : idx(i), entry(e) {}

    bool operator()(const vector<Field>& row) const {
        return row[idx] == entry;
    }
};

#endif // COMPARATORS_HPP
