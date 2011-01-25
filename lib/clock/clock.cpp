// clock.cpp
// Clocks and timers.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "clock.h"
#include <ostream>
#include <base/c_format.h>
#include <base/string_helpers.h>


namespace clocks {

  time_t Clock::time_ = ::time(0);
  tm Clock::timeinfo_ = *::localtime(&time_);


  time_t Clock::Tick() {
    time_ = ::time(0);
    LocalTime(timeinfo_, time_);
    return time_;
  }


  void Clock::Sleep(unsigned int sec) {
    ::sleep(sec);
  }


  time_t Clock::GetTimeStamp() {
    return time_;
  }


  time_t Clock::GetNormalizedTimeStamp(const unsigned long period, const long tz) {
    return NormalizeUnixTimeStamp(time_, period, tz);
  }


  const tm& Clock::GetLocalTime() {
    return timeinfo_;
  }


  int Clock::GetHour() {
    return timeinfo_.tm_hour;
  }


  int Clock::GetHourShift(const int offset) {
    if (timeinfo_.tm_hour + offset < 24) {
      return 1 << (timeinfo_.tm_hour + offset);
    }
    else {
      return 1 << (std::abs(24 - timeinfo_.tm_hour - offset) % 24);
    }
  }


  int Clock::GetWDay() {
    return timeinfo_.tm_wday;
  }


  const tm& Clock::LocalTime(tm& timeinfo, const time_t ts) {
    timeinfo = *::localtime(&ts);
    return timeinfo;
  }


  time_t Clock::NormalizeUnixTimeStamp(const time_t value, const unsigned long period, const long tz) {
    return ((value - tz) - ((value - tz) % period)) + tz;
  }


  bool Clock::HasDayChanged(const time_t x, const time_t y) {
    tm tx = *::localtime(&x);
    tm ty = *::localtime(&y);
    return tx.tm_mday != ty.tm_mday;
  }


  bool Clock::Less(const time_t x) {
    return time_ < x;
  }


  bool Clock::Equal(const tm& x) {
    return (timeinfo_.tm_sec == x.tm_sec) && (timeinfo_.tm_min == x.tm_min) &&
    (timeinfo_.tm_hour == x.tm_hour) && (timeinfo_.tm_yday == x.tm_yday);
  }


  bool Clock::Equal(const tm& x, const tm& y) {
    return (y.tm_sec == x.tm_sec) && (y.tm_min == x.tm_min) &&
    (y.tm_hour == x.tm_hour) && (y.tm_yday == x.tm_yday);
  }


  bool Clock::LessOrEqual(const time_t x) {
    return time_ <= x;
  }


  bool Clock::More(const time_t x) {
    return !LessOrEqual(x);
  }


  bool Clock::MoreOrEqual(const time_t x) {
    return !Less(x);
  }


  std::string Clock::ToString() {
    return base::ToString(time_);
  }


  std::string Clock::ToHumanReadableString(const char* format) {
    char buffer[255];
    if (::strftime(buffer, 255, format, &timeinfo_) == 0) {
      return std::string();
    }
    else {
      return std::string(buffer);
    }
  }


  std::string Clock::ToHumanReadableString(const time_t t, const char* format) {
    char buffer[255];
    tm timeinfo;

    if (::strftime(buffer, 255, format, &LocalTime(timeinfo, t)) == 0) {
      return std::string();
    }
    else {
      return std::string(buffer);
    }
  }


  std::string Clock::GetHumanReadableDuration(const unsigned int duration) {
    unsigned int d = duration;
    unsigned int days = d / 86400;
    d -= days * 86400;
    unsigned int hours = d / 3600;
    d -= hours * 3600;
    unsigned int minutes = d / 60;
    d -= minutes * 60;
    unsigned int seconds = d;
    return c_format("%ud %uh %um %us", days, hours, minutes, seconds);
  }


  std::ostream& operator<<(std::ostream& os, const Clock& t) {
    os << t.ToString();
    return os;
  }

} // namespace clocks
