// str.h
// CString library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.


#ifndef CSTRING_STR_H__
#define CSTRING_STR_H__

#include "strlcpy.h"
#include "strlcat.h"


inline void strreverse(char *begin, char *end) {
  char aux;
  while (end > begin) {
    aux = *end, *end-- = *begin, *begin++ = aux;
  }
}


inline bool strempty(const char *s) {
  return s == 0 || *s == '\0';
}


inline void substr(const char *s, char *d, unsigned begin, unsigned end) {
  *d = '\0';

  if (begin >= end) {
    return;
  }

  unsigned i = 0;
  while (begin <= end) {
    d[i++] = s[begin++];
  }
}

#endif // CSTRING_STR_H__
