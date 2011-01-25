// hirestimer.cpp
// Clocks and timers.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "hirestimer.h"

namespace clocks {

#ifdef HAVE_MACH_ABSOLUTE_TIME
  extern mach_timebase_info_data_t HiResTimer::timebase_;
#endif


  std::ostream& operator<< (std::ostream& os, const HiResTimer& t) {
    os << t.ToString();
    return os;
  }


  time_t HiResTimer::GetStartSeconds() const {
    return sec0_;
  }


  long HiResTimer::GetStartNano() const {
    return nsec0_;
  }


  time_t HiResTimer::GetMarkSeconds() const {
    return sec1_;
  }


  long HiResTimer::GetMarkNano() const {
    return nsec1_;
  }


  time_t HiResTimer::GetDifferenceSeconds() const {
    return Difference().tv_sec;
  }


  long HiResTimer::GetDifferenceNanoseconds() const {
    return Difference().tv_nsec;
  }


  void HiResTimer::Reset() {
    Set_(sec0_, nsec0_);
  }


  const HiResTimer& HiResTimer::Mark() {
    Set_(sec1_, nsec1_);
    return *this;
  }


  timespec HiResTimer::Difference() const {
    long n = nsec1_ - nsec0_;
    timespec t;
    if (n < 0) {
      t.tv_sec = sec1_ - sec0_ - 1;
      t.tv_nsec = nanoclock::second + n;
    }
    else {
      t.tv_sec = sec1_ - sec0_;
      t.tv_nsec = n;
    }
    return t;
  }


  void HiResTimer::Constructor_() {
#ifdef HAVE_MACH_ABSOLUTE_TIME
    if (timebase_.denom == 0) {
      ::mach_timebase_info(&timebase_);
    }
#endif
    Reset();
  }


  void HiResTimer::Set_(time_t& sec, long& nsec) {
#ifdef HAVE_CLOCK_GETTIME
    timespec t;
    if (::clock_gettime(CLOCK_MONOTONIC, &t) != 0) {
      base_throw(InternalError, ::strerror(errno));
    }
    sec = t.tv_sec;
    nsec = t.tv_nsec;
#elif defined HAVE_MACH_ABSOLUTE_TIME
    uint64_t now = ::mach_absolute_time();
    uint64_t mach_nano = now * timebase_.numer / timebase_.denom;
    sec = mach_nano / nanoclock::second;
    nsec = mach_nano - sec * nanoclock::second;
#else // GETTIMEOFDAY
    timeval t;
    if (::gettimeofday(&t, 0) != 0) {
      base_throw(InternalError, ::strerror(errno));
    }
    sec = t.tv_sec;
    nsec = t.tv_usec * 1000;
#endif
  }


  std::string HiResTimer::ToString() const {
    timespec t = Difference();
    std::stringstream str;
    str.fill('0');
    str << t.tv_sec << "." << std::setw(9) << t.tv_nsec;
    return str.str();
  }


  std::string HiResTimer::ToHumanReadableString() const {
    timespec t = Difference();

    time_t days = t.tv_sec / 86400;
    t.tv_sec -= days * 86400;
    time_t hours = t.tv_sec / 3600;
    t.tv_sec -= hours * 3600;
    time_t minutes = t.tv_sec / 60;
    t.tv_sec -= minutes * 60;
    time_t seconds = t.tv_sec;

    std::stringstream ss;

    if (days != 0) {
      ss << days << "d ";
    }

    if (hours != 0) {
      ss << hours << "h ";
    }

    if (minutes != 0) {
      ss << minutes << "m ";
    }

    ss << seconds << "s " << t.tv_nsec << "ns";

    return ss.str();
  }

} // namespace clocks
