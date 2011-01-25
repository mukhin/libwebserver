// hiresclock.h
// Clocks and timers.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef CLOCK_HIRESCLOCK_H__
#define CLOCK_HIRESCLOCK_H__

#include "helpers.h"

#include <base/exception.h>
#include <base/string_helpers.h>
#include <ctime>
#include <cerrno> // for errno
#include <ostream>
#include <sstream>
#include <iomanip> // for std::setw

extern "C" {
#ifdef HAVE_MACH_MACH_TIME_H
# include <mach/mach.h>
# include <mach/clock.h>
# include <mach/mach_error.h>
#else
# include <sys/time.h> // for ::gettimeofday
#endif
}


namespace clocks {

  class HiResClock {
  public:
    HiResClock() : sec_(), nsec_() { Constructor_(); }

    HiResClock& operator+ (const HiResClock& src);
    HiResClock& operator- (const HiResClock& src);

    void Now();
    void Advance(const time_t sec, const int nsec);

    static void Sleep(const unsigned int sec, const unsigned int nsec);

    std::string ToString() const;
    timespec ToTimespec() const;

    time_t GetSeconds() const;
    long GetNano() const;

  private:
    void Constructor_();

    time_t sec_;
    long nsec_;
#ifdef HAVE_CLOCK_GET_TIME
    clock_serv_t clock_;
#endif
  };

} // namespace clocks

#endif // CLOCK_HIRESCLOCK_H__
