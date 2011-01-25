// suppressedfileappender.cpp
// Log4cpp customizations.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#include "suppressedfileappender.h"
#include <base/string_helpers.h>

namespace base {

  SuppressedFileAppender::SuppressedFileAppender(const std::string& name,
                                                 const std::string& fileName,
                                                 const bool append,
                                                 const mode_t mode)
  : log4cpp::FileAppender(name, fileName, append, mode)
  , count_(0) { }


  SuppressedFileAppender::~SuppressedFileAppender() { }


  void SuppressedFileAppender::_append(const log4cpp::LoggingEvent& event) {
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
