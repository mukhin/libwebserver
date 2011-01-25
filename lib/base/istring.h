// istring.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef BASE_ISTRING_H__
#define BASE_ISTRING_H__

#include <algorithm> // for std::transform
#include <string>
#include <strings.h>
#include <sstream>
#include <vector>


namespace base {

  // @interface

  class i_char_traits : public std::char_traits<char> {
  public:
    static bool eq(char c1, char c2) {
      return ::tolower(c1) == ::tolower(c2);
    }

    static bool ne(char c1, char c2) {
      return ::tolower(c1) != ::tolower(c2);
    }

    static bool lt(char c1, char c2) {
      return ::tolower(c1) < ::tolower(c2);
    }

    static int compare(const char* s1, const char* s2, size_t n) {
      return ::strncasecmp(s1, s2, n);
    }

    static const char* find(const char* s, int n, char a) {
      while(n-- > 0 && ::tolower(*s) != ::tolower(a)) {
        ++s;
      }
      return s;
    }
  };



  // @implementation

  typedef std::basic_string<char, i_char_traits> istring;

  inline std::ostream& operator<<(std::ostream& os, const istring& s) {
    return os << std::string(s.c_str(), s.length());
  }

  template <typename T>
  base::istring ToIString(T val) {
    std::ostringstream ost;
    ost << val;
    return base::istring(ost.str().c_str(), ost.str().length());
  }

  template <typename T>
  inline bool FromIString(const base::istring& str, T& val) {
    std::istringstream ist(std::string(str.c_str(), str.length()));

    if (ist >> val) {
      return true;
    }

    return false;
  }

  template <typename T>
  inline T FromIString(const base::istring& str) {
    std::istringstream ist(std::string(str.c_str(), str.length()));
    T val;
    ist >> val;
    return val;
  }

  inline void Tokenize(const base::istring& str, std::vector<base::istring>& tokens,
                       const base::istring& delimiters = " ") {
    // Skip delimiters at beginning.
    std::string::size_type last = str.find_first_not_of(delimiters, 0);
    // Find first non-delimiter.
    std::string::size_type pos = str.find_first_of(delimiters, last);

    while (std::string::npos != pos || std::string::npos != last) {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(last, pos - last));
      // Skip delimiters.  Note the "not_of"
      last = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, last);
    }
  }

} // namespace base

#endif // BASE_ISTRING_H__
