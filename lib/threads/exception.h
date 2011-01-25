// exception.h
// Threading library.
//
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef THREADS_EXCEPTION_H__
#define THREADS_EXCEPTION_H__

#include <base/exception.h>


namespace threads {

  class ThreadException : public BaseReasonedException {
  public:
    ThreadException(const char* file, const size_t line, const std::string& init_why = "");
  };

} // namespace threads

#endif // THREADS_EXCEPTION_H__
