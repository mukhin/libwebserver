// exception.cpp
// Threading library.
//
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "exception.h"

namespace threads {

  ThreadException::ThreadException(const char* file, const size_t line, const std::string& init_why)
  : BaseReasonedException("ThreadException", file, line, init_why) { }

} // namespace threads