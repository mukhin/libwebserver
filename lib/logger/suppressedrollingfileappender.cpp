// suppressedrollingfileappender.cpp
// Log4cpp customizations.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#include "suppressedrollingfileappender.h"
#include <base/string_helpers.h>


namespace base {

  SuppressedRollingFileAppender::SuppressedRollingFileAppender(const std::string& name,
                                                 const std::string& fileName,
                                                 size_t maxFileSize,
                                                 unsigned int maxBackupIndex,
                                                 const bool append,
                                                 const mode_t mode)
  : log4cpp::RollingFileAppender(name, fileName, maxFileSize, maxBackupIndex, append, mode)
  , count_(0) { }

  SuppressedRollingFileAppender::~SuppressedRollingFileAppender() { }

  void SuppressedRollingFileAppender::_append(const log4cpp::LoggingEvent& event) {
    if (message_ != event.message) {
      if (count_ != 0) {
        std::string message = "Last message was repeated " + base::ToString(count_) + " times";
        log4cpp::LoggingEvent repeat(category_, message, ndc_, priority_);
        log4cpp::FileAppender::_append(repeat);
      }

      count_ = 0;
      category_ = event.categoryName;
      message_ = event.message;
      ndc_ = event.ndc;
      priority_ = event.priority;
      log4cpp::FileAppender::_append(event);
    }
    else {
      ++count_;
    }
  }

} // namespace base
