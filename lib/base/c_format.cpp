// c_format.cpp
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "c_format.h"
#include <cstdio>

extern "C"  {
#include <stdlib.h>
}


int arg_count() {
  return 0;
}

void c_format_validate(const char* fmt, int exp_count) {
  const char *p = fmt;
  int state = 0;
  int count = 0;

  while(*p != 0) {
    if (state == 0) {
      if (*p == '%') {
        count++;
        state = 1;
      }
    }
    else {
      switch (*p) {
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
        case 'D':
        case 'O':
        case 'U':
        case 'e':
        case 'E':
        case 'f':
        case 'g':
        case 'G':
        case 'c':
        case 's':
        case 'p':
          // parameter type specifiers
          state = 0;
          break;
        case '%':
          // escaped percent
          state = 0;
          count--;
          break;
        case 'n':
          // we don't permit %n
          fprintf(stderr, "%%n detected in c_format()\n");
          abort();
        case '*':
          // field width or precision also needs a parameter
          count++;
          break;
      }
    }
    p++;
  }

  if (exp_count != count) {
    abort();
  }
}


std::string do_c_format(const char* fmt, ...) {
  size_t buf_size = FORMAT_BUFSIZE; // Default buffer size
  vector<char> b(buf_size);
  va_list ap;

  do {
    va_start(ap, fmt);
    int ret = vsnprintf(&b[0], buf_size, fmt, ap);
    if (static_cast<size_t>(ret) < buf_size) {
      string r = string(&b[0]); // Buffer size is OK
      va_end(ap);
      return r;
    }

    buf_size = ret + 1; // Add space for the extra '\0'
    b.resize(buf_size);
  } while (true);
}
