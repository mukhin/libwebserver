// string_helpers.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef BASE_STRING_HELPERS_H__
#define BASE_STRING_HELPERS_H__

#include "istring.h"

#include <algorithm> // for std::transform
#include <cctype> // for std::tolower
#include <cstring/str.h>
#include <functional> // for std::ptr_fun
#include <map>
#include <locale>
#include <string>
#include <set>
#include <sstream>
#include <vector>
#include <ostream>

namespace base {

  typedef enum {
    PAIRCONTAINER_PRINT_KEYS,
    PAIRCONTAINER_PRINT_VALUES,
    PAIRCONTAINER_PRINT_BOTH
  } PairContainerPrintValue;


  template<class Iterator>
  size_t joined_output(std::ostream& put_to,
                       Iterator first,
                       Iterator last,
                       const std::string& join_with,
                       const std::string& prepend,
                       const std::string& append) {
    unsigned long count = 0;
    if (first != last) {
      Iterator curr = first++;

      while (first != last) {
        put_to << prepend << *curr << append << join_with;
        curr = first++;
        ++count;
      }

      put_to << prepend << *curr << append;
    }

    return count;
  }


  template<class Iterator>
  size_t joined_pair_output(std::ostream& put_to,
                            Iterator first,
                            Iterator last,
                            PairContainerPrintValue printwhat,
                            const std::string& join_with,
                            const std::string& separator,
                            const std::string& ifempty,
                            const std::string& prepend,
                            const std::string& append) {
    unsigned long count = 0;
    if (first != last) {
      Iterator curr = first++;

      while (first != last) {
        switch (printwhat) {
          case PAIRCONTAINER_PRINT_KEYS:
            put_to << prepend << curr->first << append << join_with;
            break;
          case PAIRCONTAINER_PRINT_VALUES:
            put_to << prepend << curr->second << append << join_with;
            break;
          default:
            put_to << prepend << curr->first << separator << curr->second << append << join_with;
            break;
        }

        curr = first++;
        ++count;
      }

      if (printwhat) {
        put_to << prepend << curr->second << append;
      }
      else {
        put_to << prepend << curr->first << append;
      }

      switch (printwhat) {
        case PAIRCONTAINER_PRINT_KEYS:
          put_to << prepend << curr->first << append;
          break;
        case PAIRCONTAINER_PRINT_VALUES:
          put_to << prepend << curr->second << append;
          break;
        default:
          put_to << prepend << curr->first << separator << curr->second << append;
          break;
      }
    }

    return count;
  }




  template <class Iterator>
  size_t PairContainerToString(std::string& str,
                               Iterator begin,
                               Iterator end,
                               PairContainerPrintValue printwhat,
                               const std::string& join_with = ", ",
                               const std::string& separator = ": ",
                               const std::string& ifempty = "",
                               const std::string& prepend = "",
                               const std::string& append = "") {
    if (begin == end) {
      str.append(ifempty);
      return 0;
    }

    std::ostringstream ost;
    const unsigned long count = joined_pair_output(ost, begin, end, printwhat, join_with, separator, prepend, append);
    str.append(ost.str());
    return count;
  }


  template <class Iterator>
  size_t ContainerToString(std::string& str,
                           Iterator begin,
                           Iterator end,
                           const std::string& join_with = ", ",
                           const std::string& ifempty = "",
                           const std::string& prepend = "",
                           const std::string& append = "") {
    if (begin == end) {
      str.append(ifempty);
      return 0;
    }

    std::ostringstream ost;
    const unsigned long count = joined_output(ost, begin, end, join_with, prepend, append);
    str.append(ost.str());
    return count;
  }


  template <typename T>
  std::string ToString(const T val) {
    std::ostringstream ost;
    ost << val;
    return ost.str();
  }


  template <typename T>
  inline T FromString(const std::string& str) {
    std::istringstream ist(str);
    T val;
    ist >> val;
    return val;
  }


  template <typename T>
  bool FromString(const char* str, T& val) {
    std::istringstream ist(str);

    if (ist >> val) {
      return true;
    }

    return false;
  }


  template <typename T>
  inline bool FromString(const std::string& str, T& val) {
    std::istringstream ist(str);

    if (ist >> val) {
      return true;
    }

    return false;
  }


  inline void ToLower(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), std::ptr_fun<int, int>(std::tolower));
  }


  inline void ToUpper(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), std::ptr_fun<int, int>(std::toupper));
  }


  inline void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ") {
    // Skip delimiters at beginning.
    std::string::size_type last = str.find_first_not_of(delimiters, 0);
    // Find first non-delimiter.
    std::string::size_type pos = str.find_first_of(delimiters, last);

    while (std::string::npos != pos || std::string::npos != last) {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(last, pos - last));
      // Skip delimiters. Note the "not_of"
      last = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, last);
    }
  }


  inline void Trim(std::string& str) {
    std::string::size_type pos = str.find_last_not_of(' ');
    if (pos != std::string::npos) {
      str.erase(pos + 1);
      pos = str.find_first_not_of(' ');
      if (pos != std::string::npos) {
        str.erase(0, pos);
      }
    }
    else {
      str.erase(str.begin(), str.end());
    }
  }


  inline std::string char2hex(char dec) {
    int dig1 = (dec & 0xF0) >> 4;
    int dig2 = dec & 0x0F;

    if (0 <= dig1 && dig1 <=  9) {
      dig1 += 48; // 0,48 in ascii
    }

    if (10 <= dig1 && dig1 <= 15) {
      dig1 += 97 - 10; // a,97 in ascii
    }

    if (0 <= dig2 && dig2 <=  9) {
      dig2 += 48;
    }

    if (10 <= dig2 && dig2 <= 15) {
      dig2 += 97 - 10;
    }

    const char c1 = static_cast<char>(dig1);
    const char c2 = static_cast<char>(dig2);

    std::string r;
    r.append(&c1, 1);
    r.append(&c2, 1);
    return r;
  }


  inline std::string UrlEncode(const std::string& src) {
    std::string escaped;
    size_t max = src.length();

    for(size_t i = 0; i < max; i++) {
      if ((48 <= src[i] && src[i] <= 57) ||  // 0-9
          (65 <= src[i] && src[i] <= 90) ||  // abc...xyz
          (97 <= src[i] && src[i] <= 122) || // ABC...XYZ
          (src[i]=='~' || src[i]=='!' || src[i]=='*' || src[i]=='(' || src[i]==')' || src[i]=='\'')) {
        escaped.append(&src[i], 1);
      }
      else {
        escaped.append("%");
        escaped.append(char2hex(src[i]));
      }
    }

    return escaped;
  }

} // namespace base

#endif // BASE_STRING_HELPERS_H__
