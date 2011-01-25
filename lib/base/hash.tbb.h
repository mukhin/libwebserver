// hash.tbb.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef BASE_HASH_TBB_H__
#define BASE_HASH_TBB_H__

#include "equal.h"
#include "hash.h"


namespace base {

  template<class T>
  class TbbHasher {
  public:
    static size_t hash(const T& x) {
      return Hash<T>::hash(x);
    }

    static bool equal(const T& x, const T& y) {
      return Equal<T>::equal(x, y);
    }
  };

} // namespace base

#endif // BASE_HASH_TBB_H__
