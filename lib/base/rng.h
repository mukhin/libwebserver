// rng.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef BASE_RNG_H__
#define BASE_RNG_H__

#include <cstdlib>

namespace base {

  class RandomNumberGenerator {
  public:
    ptrdiff_t operator()() const;
    ptrdiff_t operator()(const ptrdiff_t i) const;

    static bool URandomSeed(const unsigned int seed);
    static void SRandomSeed(const unsigned int seed);
    static ptrdiff_t Random();
    static ptrdiff_t Random(const ptrdiff_t i);

  private:
    static char bytes_[256];
  };

} // namespace base

#endif // BASE_RNG_H__
