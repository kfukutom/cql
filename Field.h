// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

// EECS 281 Project 3 SillyQL Field class
// Copyright 2025, Regents of the University of Michigan

#pragma once

#include <utility>
#include <string>
#include <iosfwd>
#include <cassert>
#include <exception>


// The type contained within a Field; you can use this also.
enum class ColumnType : uint8_t { String, Double, Int, Bool };


class Field {
  public:
  /* Field can be constructed out of a string, an int, a double, or a bool.
   * Don't worry about the string&& constructor. This is a C++11 move
   * constructor. Understanding how these work is not required for this project.
   */
  explicit Field(const char*);
  explicit Field(const std::string&);
  explicit Field(std::string&&);
  explicit Field(double);
  explicit Field(int);
  explicit Field(bool);
  // You can also copy construct a Field. Don't worry about Field&&
  Field(const Field&);
  Field(Field&&) noexcept;
  // Please don't be assigning
  Field& operator=(const Field&) = delete;
  Field& operator=(Field&&) = delete;

  // IAMA destructor AMA
  ~Field() noexcept;

  /* Comparisons with other Field.
   * Don't try to compare Field containing different types.
   * If you don't pass -DNDEBUG to g++ the compilation process (make debug with
   * the provided Makefile doesn't do this, but make release does), this will
   * cause an assertion to fail, which you can debug by using gdb (or your
   * favorite visual debugger) and looking at the stacktrace. Asserts are
   * removed if you compile with -DNDEBUG, and comparing different types causes
   * undefined behavior, which makes the autograder cry.
   */
  bool operator<(const Field&) const noexcept;
  bool operator>(const Field&) const noexcept;
  bool operator==(const Field&) const noexcept;
  bool operator!=(const Field&) const noexcept;

  /* Comparisons against internal type for convenience. These allow you to use
  * Field as if they were the actul types stored in them. If you try to
  * compare a Field with a type that is not the same as the internal type,
  * an assertion will trigger in the same way as for the above comparisons
  *
  * If you try to compare against a type other than the ones representable by
  * Field, you will get a compiler error. That's what this gigantic
  * template/using is for. Don't worry about how it works.
  */
  template <typename T>
  // When c++14 becomes more used, change to decay_t and enable_if_t
  using Only_allow_for_table_types_t = typename std::enable_if<
      std::is_same<typename std::decay<T>::type, double>::value ||
      std::is_same<typename std::decay<T>::type, int>::value ||
      std::is_same<typename std::decay<T>::type, bool>::value ||
      std::is_same<typename std::decay<T>::type, std::string>::value>::type;
  template <typename T, typename = Only_allow_for_table_types_t<T>>
  bool operator<(const T&) const noexcept;
  template <typename T, typename = Only_allow_for_table_types_t<T>>
  bool operator>(const T&) const noexcept;
  template <typename T, typename = Only_allow_for_table_types_t<T>>
  bool operator==(const T&) const noexcept;
  template <typename T, typename = Only_allow_for_table_types_t<T>>
  bool operator!=(const T&) const noexcept;

  private:
  // Stop looking at my privates. You won't like what you see.
  // Skip down to after the end of the class for some other things

  ColumnType tag;
  union {
    std::string data_string;
    double data_double;
    int data_int;
    bool data_bool;
  };

  friend struct std::hash<Field>;
  friend std::ostream& operator<<(std::ostream&, const Field&);

  /*
    fast_pass taken from facebook's fatal library.
    written by Marcelo Juchem <marcelo@fb.com>
    https://github.com/facebook/fatal/blob/master/fatal/type/fast_pass.h

    Modified slightly to apply here
    change to decay_t, add_const_t, condtional_t, and bool_constant as they
    become available.
    tl/dr if it's a scalar type, pass in as is, otherwise use a const&
  */
  template <typename T>
  using is_fast_pass = std::integral_constant<
      bool,
      std::is_scalar<typename std::decay<T>::type>::value>;
  template <typename T>
  using fast_pass = typename std::conditional<
      is_fast_pass<typename std::decay<T>::type>::value,
      typename std::decay<T>::type,
      typename std::add_lvalue_reference<typename std::add_const<
          typename std::decay<T>::type>::type>::type>::type;


  // as<T> operator to use the Field as if it were type T. Only implemented
  // for the types allowed as Field and returns a const& for string.
  template <typename T> fast_pass<T> as() const noexcept;

  // helper for comparisons with other Field.
  // Change to template <template <typename> typename COMP> in c++17
  template <template <typename> class COMP>
  bool compare(const Field&) const noexcept;

  // Helper to dedupe constructor code
  template <typename TT> void construct_from(TT&&);

}; // Field

// This is so you don't have to pass a hash function to the unordered_map
// template. Now the unordered_map understands what to do with a Field
// basically you can just do unordered_map<Field, othertype> and the hashing
// just magically works
//
// Note: Don't normally put things in the standard namespace.
namespace std {
template <> struct hash<Field> {
  size_t operator()(const Field&) const noexcept;
};
}

// You can print Field, it just prints the internal value.
// So Field tt(11); cout << tt; prints "11" (without quotes)
std::ostream& operator<<(std::ostream&, const Field&);

// These allow you to compare Field with whatever types they represent in
// both orders (i.e. tt < 7 as well as 7 > tt).
template <typename T> bool operator<(const T& t, const Field& tt) noexcept;
template <typename T> bool operator>(const T& t, const Field& tt) noexcept;
template <typename T> bool operator==(const T& t, const Field& tt) noexcept;
template <typename T> bool operator!=(const T& t, const Field& tt) noexcept;


////////////////////////////////////////////////////////////////////////////////
/*        Implementations here. Stop reading if you value your sanity.        */
/* But seriously, you don't have to look at or understand anything below here */
////////////////////////////////////////////////////////////////////////////////


template <typename T> bool operator<(const T& t, const Field& tt) noexcept {
  return tt > t;
}
template <typename T> bool operator>(const T& t, const Field& tt) noexcept {
  return tt < t;
}
template <typename T>
bool operator==(const T& t, const Field& tt) noexcept {
  return tt == t;
}
template <typename T>
bool operator!=(const T& t, const Field& tt) noexcept {
  return tt != t;
}

template <typename T, typename>
bool Field::operator<(const T& t) const noexcept {
  return as<T>() < t;
}
template <typename T, typename>
bool Field::operator>(const T& t) const noexcept {
  return as<T>() > t;
}
template <typename T, typename>
bool Field::operator==(const T& t) const noexcept {
  return as<T>() == t;
}
template <typename T, typename>
bool Field::operator!=(const T& t) const noexcept {
  return as<T>() != t;
}

// Helper to dedupe comparison operators
template <template <typename> class COMP>
bool Field::compare(const Field& other) const noexcept {
  assert(tag == other.tag &&
         "tried to compare Field containing different types");
  switch (tag) {
  case ColumnType::String:
    return COMP<std::string>{}(data_string, other.data_string);
  case ColumnType::Double:
    return COMP<double>{}(data_double, other.data_double);
  case ColumnType::Int:
    return COMP<int>{}(data_int, other.data_int);
  case ColumnType::Bool:
    return COMP<bool>{}(data_bool, other.data_bool);
  }
  std::terminate();
}

// Helper to dedupe constructor code
template <typename TT> void Field::construct_from(TT&& other) {
  switch (tag) {
  case ColumnType::String:
    // this is a hack bc a const&& still can't be moved from
    new (&data_string) std::string{std::move(other.data_string)};
    break;
  case ColumnType::Double:
    data_double = other.data_double;
    break;
  case ColumnType::Int:
    data_int = other.data_int;
    break;
  case ColumnType::Bool:
    data_bool = other.data_bool;
    break;
  }
}
