// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#ifndef COMPARATORS_HPP
#define COMPARATORS_HPP

/*
This file is for the BST implementation of my code in 
main.cpp + Database.cpp / Database.h implementation.
These are functors / comparators for the BST std::map<>
*/

template <typename T>
struct greater {
    bool operator()(const T& a, const T& b) const {
        return a > b;
    }
}; // greater()

template <typename T>
struct smaller {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs < rhs;
    } // operator
}; // smaller()

#endif // COMPARATORS_HPP