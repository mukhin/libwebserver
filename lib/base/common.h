// common.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef BASE_COMMON_H__
#define BASE_COMMON_H__

template <typename T>
inline void destroy(T*& ptr) {
  if (ptr) {
    delete ptr;
    ptr = 0;
  }
}

#endif // BASE_COMMON_H__
