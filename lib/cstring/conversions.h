// conversions.h
// CString library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef CSTRING_CONVERSIONS_H__
#define CSTRING_CONVERSIONS_H__

#include <base/exception.h>
#include <inttypes.h>
#include <limits.h>


namespace base {

  template <typename T>
  unsigned int ToLowerCString(T value, char* str, const int base) {
    static char alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    return IntToAlpha(value, str, base, alphabet);
  }

  template <typename T>
  unsigned int ToUpperCString(T value, char* str, const int base) {
    static char alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return IntToAlpha(value, str, base, alphabet);
  }

  template<typename T>
  void Div_(const T& numerator, const T& denominator, T& quotient, T& remainder) {
    static const int bits = sizeof(T) * CHAR_BIT;

    if (denominator == 0) {
      throw std::domain_error("divide by zero");
    }
    else {
      T n = numerator;
      T d = denominator;
      T x = 1;
      T answer = 0;

      while ((n >= d) && (((d >> (bits - 1)) & 1) == 0)) {
        x <<= 1;
        d <<= 1;
      }

      while (x != 0) {
        if (n >= d) {
          n -= d;
          answer |= x;
        }

        x >>= 1;
        d >>= 1;
      }

      quotient = answer;
      remainder = n;
    }
  }

  template <typename T>
  unsigned int IntToAlpha(T value, char* str, const int base, const char* alphabet) {
    char* wstr = str;
    T sign;
    T res = value;
    unsigned int chars = 0;

    // Validate base

    if (base < 2 || base > 35) {
      *wstr = '\0';
      return chars;
    }

    // Take care of sign
    if ((sign = value) < 0) {
      value = abs(value);
    }

    do {
      T remainder;
      Div_(res, static_cast<T>(base), res, remainder);
      *wstr++ = alphabet[remainder];
      ++chars;
    } while (res != 0);

    if (sign < 0) {
      *wstr++ = '-';
      ++chars;
    }

    *wstr = '\0';

    // Reverse string
    strreverse(str, wstr - 1);
    return chars;
  }

  template <>
  inline unsigned int IntToAlpha(uint64_t value, char* str, const int base, const char* alphabet) {
    char* wstr = str;
    uint64_t res = value;
    unsigned int chars = 0;

    // Validate base
    if (base < 2 || base > 35) {
      *wstr = '\0';
      return chars;
    }

    do {
      uint64_t remainder;
      Div_(res, static_cast<uint64_t>(base), res, remainder);
      *wstr++ = alphabet[remainder];
      ++chars;
    } while (res != 0);

    *wstr = '\0';

    // Reverse string
    strreverse(str, wstr - 1);
    return chars;
  }

} // namespace base

#endif // CSTRING_CONVERSIONS_H__
