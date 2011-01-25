// cstring.cpp
// CString library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "cstring.h"
#include <string>
#include <ostream>


namespace base {

  const size_t CString::npos = static_cast<size_t>(-1);

  std::ostream& operator<< (std::ostream& os, const CString& str) {
    os << str.Str();
    return os;
  }


  base::CString operator+(const base::CString& x, const base::CString& y) {
    base::CString r(x.Length() + y.Length());
    r.Append(x);
    r.Append(y);
    return r;
  }


  CString::CString()
  : string_('\0')
  , length_(0)
  , reserved_(0) { }


  CString::CString(const size_t len)
  : length_(0)
  , reserved_(len) {
    if ((string_ = static_cast<char*>(::malloc(reserved_ + 1))) == 0) {
      base_throw(InternalError, "malloc failed");
    }
    string_[length_] = '\0';
  }


  CString::CString(const char* str, const size_t len)
  : length_(len)
  , reserved_(0) {
    if ((string_ = static_cast<char*>(::malloc(length_ + 1))) == 0) {
      base_throw(InternalError, "malloc failed");
    }

    ::memcpy(static_cast<void*>(string_), str, length_);
    string_[length_] = '\0';
  }


  CString::CString(const std::string& str)
  : length_(str.length())
  , reserved_(0) {
    if ((string_ = static_cast<char*>(::malloc(length_ + 1))) == 0) {
      base_throw(InternalError, "malloc failed");
    }

    ::memcpy(static_cast<void*>(string_), str.c_str(), length_);
    string_[length_] = '\0';
  }


  CString::CString(const CString& str)
  : length_(str.Length())
  , reserved_(0) {
    if ((string_ = static_cast<char*>(::malloc(length_ + 1))) == 0) {
      base_throw(InternalError, "malloc failed");
    }

    ::memcpy(static_cast<void*>(string_), str.Str(), length_);
    string_[length_] = '\0';
  }


  CString::~CString() {
    if (string_) {
      ::free(string_);
    }
  }


  void CString::Reserve(const size_t len) {
    if (length_ == 0 && reserved_ == 0) {
      if ((string_ = static_cast<char*>(::malloc(len + 1))) == 0) {
        base_throw(InternalError, "malloc failed");
      }
    }
    else {
      if ((string_ = static_cast<char*>(::realloc(static_cast<void*>(string_), length_ + reserved_ + len + 1))) == 0) {
        base_throw(InternalError, "realloc failed");
      }
    }

    reserved_ += len;
    string_[length_] = '\0';
  }


  CString& CString::operator=(const CString& str) {
    reserved_ += length_;
    length_ = 0;
    return Append(str);
  }


  bool CString::operator==(const CString& str) const {
    if (length_ != str.Length()) {
      return false;
    }
    else {
      return Compare(str) == 0;
    }
  }


  bool CString::operator<(const CString& str) const {
    return Compare(str) < 0;
  }


  CString& CString::Append(const CString& str) {
    return Append(str.Str(), str.Length());
  }


  CString& CString::Append(const std::string& str) {
    return Append(str.c_str(), str.length());
  }


  CString& CString::AppendChar(const char ch) {
    if (length_ == 0 && reserved_ == 0) {
      if ((string_ = static_cast<char*>(::malloc(2))) == 0) {
        base_throw(InternalError, "malloc failed");
      }
    }
    else if (reserved_ > 1) {
      reserved_ -= 2;
    }
    else { // reserved_ < 2
      if ((string_ = static_cast<char*>(::realloc(static_cast<void*>(string_), length_ + 2))) == 0) {
        base_throw(InternalError, "realloc failed");
      }

      reserved_ = 0;
    }

    string_[length_] = ch;
    length_++;
    string_[length_] = '\0';

    return *this;
  }


  CString& CString::Append(const char* str, const size_t len) {
    if (str == 0 || len == 0) {
      return *this;
    }

    if (length_ == 0 && reserved_ == 0) {
      if ((string_ = static_cast<char*>(::malloc(len + 1))) == 0) {
        base_throw(InternalError, "malloc failed");
      }
    }
    else if (reserved_ > len) {
      reserved_ -= len + 1;
    }
    else { // reserved_ < len
      if ((string_ = static_cast<char*>(::realloc(static_cast<void*>(string_), length_ + len + 1))) == 0) {
        base_throw(InternalError, "realloc failed");
      }

      reserved_ = 0;
    }

    ::memcpy(static_cast<void*>(string_ + length_), str, len);
    length_ += len;
    string_[length_] = '\0';

    return *this;
  }


  char* CString::Find(const char* str) const {
    return ::strstr(string_, str);
  }


  char* CString::Find(const CString& str) const {
    return Find(str.Str());
  }


  char* CString::Find(const char* str, const size_t pos) const {
    if (pos >= length_) {
      base_throw(InternalError, "CString size overflow");
    }

    return ::strstr(string_ + pos, str);
  }


  char* CString::Find(const CString& str, const size_t pos) const {
    return Find(str.Str(), pos);
  }


  size_t CString::Pos(const char* str) const {
    char* needle = Find(str);

    if (needle == 0) {
      return npos;
    }

    return needle - string_;
  }


  size_t CString::Pos(const CString& str) const{
    return Pos(str.Str());
  }


  size_t CString::Pos(const char* str, const size_t pos) const {
    char* needle = Find(str, pos);

    if (needle == 0) {
      return npos;
    }

    return needle - string_;
  }


  size_t CString::Pos(const CString& str, const size_t pos) const {
    return Pos(str.Str(), pos);
  }


  bool CString::Equals(const char* str, const size_t len) const {
    if (length_ < len) {
      return false;
    }
    else {
      return Compare(str, len) == 0;
    }
  }


  int CString::Compare(const char* str, const size_t len) const {
    return Compare(str, 0, len);
  }


  int CString::Compare(const CString& str) const {
    return Compare(str.Str(), str.Length());
  }


  int CString::Compare(const char* str, const size_t pos, const size_t len) const {
    if (length_ >= pos + len) {
      return ::memcmp(string_ + pos, str, len);
    }
    else {
      return ::memcmp(string_ + pos, str, length_ - pos);
    }
  }


  int CString::Compare(const CString& str, const size_t pos, const size_t len) const {
    return Compare(str.Str(), pos, len);
  }


  CString CString::Substring(const size_t pos, const size_t len) const {
    if (pos + len > length_) {
      base_throw(InternalError, "CString size overflow");
    }

    return CString(string_ + pos, len);
  }


  void CString::Erase(const size_t begin, const size_t len) {
    if (begin + len > length_) {
      base_throw(InternalError, "CString size overflow");
    }

    ::memmove(static_cast<void*>(string_ + begin), static_cast<void*>(string_ + begin + len), length_ - begin - len);
    length_ -= len;
    reserved_ += len;
    string_[length_] = '\0';
  }


  void CString::Clear() {
    if (length_ != 0) {
      reserved_ += length_;
      length_ = 0;
      string_[0] = '\0';
    }
  }


  template<class T>
  T CString::ToInteger() const {
    return ::strtol(string_, 0, 10);
  }


  template<>
  int CString::ToInteger() const {
    return static_cast<int>(::strtol(string_, 0, 10));
  }


  template<>
  long CString::ToInteger() const {
    return ::strtol(string_, 0, 10);
  }


  template<>
  long long CString::ToInteger() const {
    return ::strtoll(string_, 0, 10);
  }


  template<>
  unsigned int CString::ToInteger() const {
    return static_cast<unsigned int>(::strtoul(string_, 0, 10));
  }


  template<>
  unsigned long CString::ToInteger() const {
    return ::strtoul(string_, 0, 10);
  }


  template<>
  unsigned long long CString::ToInteger() const {
    return ::strtoull(string_, 0, 10);
  }


  template<class T>
  T CString::ToFractional() const {
    return ::strtof(string_, 0);
  }


  template<>
  float CString::ToFractional() const {
    return ::strtof(string_, 0);
  }


  template<>
  double CString::ToFractional() const {
    return ::strtod(string_, 0);
  }


  template<>
  long double CString::ToFractional() const {
    return ::strtold(string_, 0);
  }


  CString& CString::UrlDecode() {
    const char* src = string_;
    char* dst = string_;
    char tmp[3];
    tmp[2] = '\0';

    while (*src) {
      if (*src == '%' && src[1] != '\0' && src[2] != '\0') {
        src++;
        tmp[0] = *src++;
        tmp[1] = *src++;
        *dst++ = static_cast<char>(::strtol(tmp, 0, 16));
        length_ -= 2;
        reserved_ += 2;
      } else {
        *dst++ = *src++;
      }
    }
    *dst++ = '\0';

    return *this;
  }


  size_t CString::Hash() const {
    size_t h = 0;

    if (!IsEmpty()) {
      for (const char* s = string_; *s; ++s) {
        h = (h * 17) ^ *s;
      }
    }

    return h;
  }


  void CString::AdjustLength(const ssize_t len) {
    if (len == 0) {
      return;
    }
    else if (len < 0 && static_cast<size_t>(-len) > length_) {
      base_throw(InternalError, "Shrinking failed, not enough length available");
    }
    else if (len > 0 && static_cast<size_t>(len) > reserved_) {
      base_throw(InternalError, "Expansion failed, not enough reserved space available");
    }

    reserved_ = reserved_ - len;
    length_ = length_ + len;
    string_[length_] = '\0';
  }

} // namespace base


