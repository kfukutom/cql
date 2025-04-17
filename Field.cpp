#include "Field.h"

#include <functional>
#include <cassert>
#include <iostream>
#include <utility>
#include <exception>

using std::string;
using std::greater;
using std::less;
using std::equal_to;
using std::not_equal_to;
using std::move;
using std::ostream;
using std::terminate;

// elt constructors
// const char* method, because otherwise it matches the bool one
Field::Field(const char* val) : tag{ColumnType::String}, data_string{val} {}
Field::Field(const string& val) : tag{ColumnType::String}, data_string{val} {}
Field::Field(string&& val) : tag{ColumnType::String}, data_string{std::move(val)} {}
Field::Field(double val) : tag{ColumnType::Double}, data_double{val} {}
Field::Field(int val) : tag{ColumnType::Int}, data_int{val} {}
Field::Field(bool val) : tag{ColumnType::Bool}, data_bool{val} {}

// copy/move ctors
Field::Field(const Field& other) : tag{other.tag} {
  construct_from(other);
}
Field::Field(Field&& other) noexcept : tag{other.tag} {
  construct_from(std::move(other));
}

// dtor
Field::~Field() noexcept {
  if (tag == ColumnType::String)
    data_string.~string();
}

// comparisons
bool Field::operator<(const Field& other) const noexcept {
  return compare<less>(other);
}
bool Field::operator>(const Field& other) const noexcept {
  return compare<greater>(other);
}
bool Field::operator==(const Field& other) const noexcept {
  return compare<equal_to>(other);
}
bool Field::operator!=(const Field& other) const noexcept {
  return compare<not_equal_to>(other);
}

// specializations on the as<> template for conversion
template <> const string& Field::as<string>() const noexcept {
  assert(tag == ColumnType::String &&
         "tried to use Field as a string when it didn't contain a string");
  return data_string;
}
template <> double Field::as<double>() const noexcept {
  assert(tag == ColumnType::Double &&
         "tried to use Field as a double when it didn't contain a double");
  return data_double;
}
template <> int Field::as<int>() const noexcept {
  assert(tag == ColumnType::Int &&
         "tried to use Field as an int when it didn't contain an int");
  return data_int;
}
template <> bool Field::as<bool>() const noexcept {
  assert(tag == ColumnType::Bool &&
         "tried to use Field as a bool when it didn't contain a bool");
  return data_bool;
}

namespace std {
// overload to std::hash
size_t hash<Field>::operator()(const Field& tt) const noexcept {
  switch (tt.tag) {
  case ColumnType::String:
    return hash<string>{}(tt.data_string);
  case ColumnType::Double:
    return hash<double>{}(tt.data_double);
  case ColumnType::Int:
    return hash<int>{}(tt.data_int);
  case ColumnType::Bool:
    return hash<bool>{}(tt.data_bool);
  }
  terminate();
}
}

ostream& operator<<(ostream& os, const Field& tt) {
  switch (tt.tag) {
  case ColumnType::String:
    return os << tt.data_string;
  case ColumnType::Double:
    return os << tt.data_double;
  case ColumnType::Int:
    return os << tt.data_int;
  case ColumnType::Bool:
    return os << tt.data_bool;
  }
  terminate();
}
