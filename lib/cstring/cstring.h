// cstring.h
// CString library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef CSTRING_CSTRING_H__
#define CSTRING_CSTRING_H__

#include <base/exception.h>
#include <cstdlib>
#include <string>
#include "str.h"
#include "conversions.h"


namespace base {

  class CString {
  public:
    CString();
    explicit CString(const size_t len);
    explicit CString(const char* str, const size_t len);
    explicit CString(const std::string& str);
    CString(const CString& str);
    ~CString();

    void Reserve(const size_t len);

    CString& operator=(const CString& str);
    bool operator==(const CString& str) const;
    bool operator<(const CString& str) const;

    CString& Append(const char* str, const size_t len);
    CString& Append(const std::string& str);
    CString& Append(const CString& str);
    CString& AppendChar(const char ch);
    template<class T>
    CString& AppendDecimal(const T val);
    template<class T>
    CString& AppendHexadecimal(const T val, const bool uppercase, const unsigned int width = 0);

    template<class T>
    CString& AppendUppercaseInteger(const T val, const int base, const unsigned int width);
    template<class T>
    CString& AppendLowercaseInteger(const T val, const int base, const unsigned int width);
    template<class T>
    CString& AppendFractional(const T val);

    char* Find(const char* str) const;
    char* Find(const CString& str) const;
    char* Find(const char* str, const size_t pos) const;
    char* Find(const CString& str, const size_t pos) const;

    size_t Pos(const char* str) const;
    size_t Pos(const CString& str) const;
    size_t Pos(const char* str, const size_t pos) const;
    size_t Pos(const CString& str, const size_t pos) const;

    bool Equals(const char* str, const size_t len) const;

    int Compare(const char* str, const size_t len) const;
    int Compare(const CString& str) const;
    int Compare(const char* str, const size_t pos, const size_t len) const;
    int Compare(const CString& str, const size_t pos, const size_t len) const;

    CString Substring(const size_t pos, const size_t len) const;

    void Erase(const size_t begin, const size_t len);
    void Clear();

    template<class T>
    T ToInteger() const;

    template<class T>
    T ToFractional() const;

    CString& UrlDecode();

    size_t Hash() const;

    size_t Length() const;
    void AdjustLength(const ssize_t len);

    size_t Reserved() const;
    const char* Str() const;

    char* Begin() const;
    char* End() const;

    bool IsEmpty() const;

    static const size_t npos;

    inline std::string ToString(const base::CString& c_str);
  private:
    char* string_;
    size_t length_;
    size_t reserved_;
  };


  inline size_t CString::Length() const {
    return length_;
  }


  inline size_t CString::Reserved() const {
    return reserved_;
  }


  inline const char* CString::Str() const {
    return string_;
  }


  inline char* CString::Begin() const {
    return string_;
  }


  inline char* CString::End() const {
    return string_ + length_;
  }


  inline bool CString::IsEmpty() const {
    return length_ == 0 || strempty(string_);
  }


  template<class T>
  CString& CString::AppendDecimal(const T val) {
    return AppendLowercaseInteger<T>(val, 10, 0);
  }


  template<class T>
  CString& CString::AppendHexadecimal(const T val, const bool uppercase, const unsigned int width) {
    if (uppercase) {
      return AppendUppercaseInteger<T>(val, 16, width);
    }
    else {
      return AppendLowercaseInteger<T>(val, 16, width);
    }
  }


  template<class T>
  CString& CString::AppendFractional(const T val) {
    char buf[30];
    int res = snprintf(buf, 30, "%f", val);
    return Append(buf, res);
  }


  template<class T>
  CString& CString::AppendUppercaseInteger(const T val, const int base, const unsigned int width) {
    static char alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char str[21];
    size_t chars = IntToAlpha(val, str, base, alphabet);

    if (chars < width) {
      for (size_t i = 0; i < width - chars; ++i) {
        AppendChar('0');
      }
    }

    return Append(str, chars);
  }


  template<class T>
  CString& CString::AppendLowercaseInteger(const T val, const int base, const unsigned int width) {
    static char alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char str[21];
    size_t chars = IntToAlpha(val, str, base, alphabet);

    if (chars < width) {
      for (size_t i = 0; i < width - chars; ++i) {
        AppendChar('0');
      }
    }

    return Append(str, chars);
  }

  std::ostream& operator<< (std::ostream& os, const CString& str);
  base::CString operator+(const base::CString& x, const base::CString& y);

  inline std::string CString::ToString(const base::CString& c_str) {
    if (!c_str.IsEmpty()) {
      return std::string(c_str.Str());
    }
    else {
      return std::string();
    }
  }

} // namespace base

#endif // CSTRING_CSTRING_H__
