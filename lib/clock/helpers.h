// helpers.h
// Clocks and timers.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef CLOCK_HELPERS_H__
#define CLOCK_HELPERS_H__

#include <ctime>

extern "C" {
#include <unistd.h>
}


namespace clocks {

  namespace nanoclock {
    const long second = 1000000000;
    const long millisecond = 1000000;
    const long microsecond = 1000;
  }

  inline timespec MakeTimespec(long nsec) {
    long s = nsec / nanoclock::second;
    long n = nsec - s * nanoclock::second;
    timespec ts = { s, n };
    return ts;
  }

} // namespace clocks

#endif // CLOCK_HELPERS_H__
