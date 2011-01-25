// hiresclock.cpp
// Clocks and timers.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "hiresclock.h"

namespace clocks {

  time_t HiResClock::GetSeconds() const {
    return sec_;
  }


  long HiResClock::GetNano() const {
    return nsec_;
  }


  std::string HiResClock::ToString() const {
    std::stringstream str;
    str.fill('0');
    str << sec_ << "." << std::setw(9) << nsec_;
    return str.str();
  }


  timespec HiResClock::ToTimespec() const {
    timespec ts = { sec_, nsec_ };
    return ts;
  }


  HiResClock& HiResClock::operator+ (const HiResClock& src) {
    sec_ += src.sec_ + (nsec_ + src.nsec_) / nanoclock::second;
    nsec_ = (nsec_ + src.nsec_) % nanoclock::second;
    return *this;
  }


  HiResClock& HiResClock::operator- (const HiResClock& src) {
    long n = nsec_ - src.nsec_;
    if (n < 0) {
      sec_ -= src.sec_ - 1;
      nsec_ = nanoclock::second + n;
    }
    else {
      sec_ -= src.sec_;
      nsec_ = n;
    }
    return *this;
  }


  std::ostream& operator<< (std::ostream& os, const HiResClock& t) {
    os << t.ToString();
    return os;
  }


  void HiResClock::Advance(const time_t sec, const int nsec) {
    if (nsec < 0) {
      base_throw(InvalidRange, "Attempt to travel back in time.");
    }

    Now();
    sec_ += sec + (nsec_ + nsec) / nanoclock::second;
    nsec_ = (nsec_ + nsec) % nanoclock::second;
  }


  void HiResClock::Now() {
#if defined HAVE_CLOCK_GETTIME
    timespec t;
    if (::clock_gettime(CLOCK_REALTIME, &t) != 0) {
      base_throw(InternalError, ::strerror(errno));
    }
    sec_ = t.tv_sec;
    nsec_ = t.tv_nsec;
#elif defined HAVE_CLOCK_GET_TIME
    mach_timespec_t t;
    kern_return_t ret = ::clock_get_time(clock_, &t);
    if (ret != KERN_SUCCESS) {
      base_throw(InternalError, "clock_get_time() failed: " + base::ToString(::mach_error_string(ret)));
    }
    sec_ = t.tv_sec;
    nsec_ = t.tv_nsec;
#else // GETTIMEOFDAY
    timeval t;
    if (::gettimeofday(&t, 0) != 0) {
      base_throw(InternalError, ::strerror(errno));
    }
    sec_ = t.tv_sec;
    nsec_ = t.tv_usec * 1000;
#endif
  }


  void HiResClock::Sleep(const unsigned int sec, const unsigned int nsec) {
    if (nsec >= static_cast<unsigned int>(nanoclock::second)) {
      base_throw(InvalidRange, "Invalid nanoseconds value, should be less than 1e9 (one second).");
    }

    timespec ts = { sec, nsec };
    ::nanosleep(&ts, 0);
  }


  void HiResClock::Constructor_() {
#ifdef HAVE_CLOCK_GET_TIME
    kern_return_t ret = ::host_get_clock_service(::mach_host_self(), CALENDAR_CLOCK, &clock_);
    if (ret != KERN_SUCCESS) {
      base_throw(InternalError, "host_get_clock_service() failed: " + base::ToString(::mach_error_string(ret)));
    }
#endif
    Now();
  }

} // namespace clocks
