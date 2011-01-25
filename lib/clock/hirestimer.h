// hirestimer.h
// Clocks and timers.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef CLOCK_HIRESTIMER_H__
#define CLOCK_HIRESTIMER_H__

#include "hiresclock.h"

#include <inttypes.h>
#include <iomanip> // for std::setw
#include <ostream>
#include <sstream>


extern "C" {
#ifdef HAVE_MACH_MACH_TIME_H
# include <mach/mach_time.h> // for ::mach_absolute_time, mach_timebase_info_data_t
#else
# include <sys/time.h> // for ::gettimeofday
#endif
}


namespace clocks {

  class HiResTimer {
  public:
    HiResTimer()
    : sec0_(0)
    , sec1_(0)
    , nsec0_(0)
    , nsec1_(0) {
      Constructor_();
    }

    void Reset();
    const HiResTimer& Mark();

    time_t GetStartSeconds() const;
    long GetStartNano() const;
    time_t GetMarkSeconds() const;
    long GetMarkNano() const;

    timespec Difference() const;
    time_t GetDifferenceSeconds() const;
    long GetDifferenceNanoseconds() const;
    std::string ToString() const;
    std::string ToHumanReadableString() const;

  private:
    void Constructor_();
    void Set_(time_t& sec, long& nsec);

    time_t sec0_;
    time_t sec1_;
    long nsec0_;
    long nsec1_;

#ifdef HAVE_MACH_ABSOLUTE_TIME
    static mach_timebase_info_data_t timebase_;
#endif
  };

  std::ostream& operator<< (std::ostream& os, const HiResTimer& t);

} // namespace clocks

#endif // CLOCK_HIRESTIMER_H__
