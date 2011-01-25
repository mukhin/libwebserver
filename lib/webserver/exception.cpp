// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "exception.h"

namespace webserver {

  DeserializationError::DeserializationError(const char* file, const size_t line, const std::string& init_why)
  : BaseReasonedException("Deserialization failed", file, line, init_why) { }

  TooManyConnectionsError::TooManyConnectionsError(const char* file, const size_t line, const std::string& init_why)
  : BaseReasonedException("Too many connections", file, line, init_why) { }

} // namespace webserver
