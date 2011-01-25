// clock.h
// Clocks and timers.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef CLOCK_CLOCK_H__
#define CLOCK_CLOCK_H__

#include <ctime>
#include <string>


namespace clocks {

  class Clock {
  public:
    static time_t Tick();
    static void Sleep(unsigned int sec);

    static time_t GetTimeStamp();
    static time_t GetNormalizedTimeStamp(const unsigned long period, const long tz = 0);
    static const tm& GetLocalTime();

    static int GetHour();
    static int GetHourShift(const int offset = 0);
    static int GetWDay();

    static const tm& LocalTime(tm& timeinfo, const time_t ts);
    static time_t NormalizeUnixTimeStamp(const time_t value, const unsigned long period, const long tz = 0);

    static bool HasDayChanged(const time_t x, const time_t y);
    static bool Equal(const tm& x);
    static bool Equal(const tm& x, const tm& y);
    static bool Less(const time_t x);
    static bool LessOrEqual(const time_t x);
    static bool More(const time_t x);
    static bool MoreOrEqual(const time_t x);

    static std::string ToString();
    static std::string ToHumanReadableString(const char* format = "%a %b %e %H:%M:%S %Z %Y");
    static std::string ToHumanReadableString(const time_t t, const char* format = "%a %b %e %H:%M:%S %Z %Y");
    static std::string GetHumanReadableDuration(const unsigned int duration);

  private:
    static time_t time_;
    static tm timeinfo_;
  };

  std::ostream& operator<<(std::ostream& os, const Clock& t);

} // namespace clocks

#endif // CLOCK_CLOCK_H__
