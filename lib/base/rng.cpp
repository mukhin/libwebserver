// rng.cpp
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "rng.h"
#include <cstdio>

namespace base {

  char RandomNumberGenerator::bytes_[256];

  bool RandomNumberGenerator::URandomSeed(const unsigned int seed) {
    FILE* urandom = ::fopen("/dev/urandom", "r");
    if (urandom) {
      size_t bytes_read = ::fread(bytes_, 1, sizeof(bytes_), urandom);

      if (bytes_read == sizeof(bytes_)) {
        ::initstate(seed, bytes_, sizeof(bytes_));
        ::setstate(bytes_);
      }

      ::fclose(urandom);
      return true;
    }
    else {
      return false;
    }
  }

  void RandomNumberGenerator::SRandomSeed(const unsigned int seed) {
    ::srandom(seed);
  }


  ptrdiff_t RandomNumberGenerator::Random() {
    return static_cast<ptrdiff_t>(::random());
  }


  ptrdiff_t RandomNumberGenerator::Random(const ptrdiff_t i) {
    return static_cast<ptrdiff_t>(::random() % i);
  }

  ptrdiff_t RandomNumberGenerator::operator()() const {
    return Random();
  }


  ptrdiff_t RandomNumberGenerator::operator()(const ptrdiff_t i) const {
    return Random(i);
  }

} // namespace base
