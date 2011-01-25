// suppressedfileappender.h
// Log4cpp customizations.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#ifndef BASE__SUPPRESSED_FILE_APPENDER_H__
#define BASE__SUPPRESSED_FILE_APPENDER_H__

#include <inttypes.h>
#include <log4cpp/FileAppender.hh>
#include <map>
#include <string>

namespace base {

  class SuppressedFileAppender : public log4cpp::FileAppender {
  public:
    SuppressedFileAppender(const std::string& name,
                           const std::string& fileName,
                           const bool append = true,
                           const mode_t mode = 00644);

    virtual ~SuppressedFileAppender();

  protected:
    virtual void _append(const log4cpp::LoggingEvent& event);

  private:
    std::string category_;
    std::string message_;
    std::string ndc_;
    log4cpp::Priority::Value priority_;
    uint32_t count_;
  };

} // namespace base

#endif // BASE__SUPPRESSED_FILE_APPENDER_H__
