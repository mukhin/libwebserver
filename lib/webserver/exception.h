// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef WEBSERVER_EXCEPTION_H__
#define WEBSERVER_EXCEPTION_H__

#include <base/exception.h>

namespace webserver {

  class DeserializationError : public BaseReasonedException {
  public:
    DeserializationError(const char* file, const size_t line, const std::string& init_why = "");
  };

  class TooManyConnectionsError : public BaseReasonedException {
  public:
    TooManyConnectionsError(const char* file, const size_t line, const std::string& init_why = "");
  };

} // namespace webserver

#endif // WEBSERVER_EXCEPTION_H__
