// exception.cpp
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "exception.h"
#include "c_format.h"

BaseException::BaseException(const char* init_what, const char* file, const size_t line)
: std::runtime_error(init_what)
, what_(init_what)
, file_(file)
, line_(line) { }


BaseException::BaseException(const BaseException& src)
: std::runtime_error(src.what_)
, what_(src.what_)
, file_(src.file_)
, line_(src.line_) { }


BaseException::~BaseException() throw() { }


BaseException& BaseException::operator=(const BaseException& src) {
  what_ = src.what_;
  file_ = src.file_;
  line_ = src.line_;
  return *this;
}


std::string BaseException::message() const throw() {
  return what_;
}


const std::string BaseException::where() const {
  return c_format("line %lu of %s", line_, file_);
}


const std::string BaseException::why() const {
  return "Not specified";
}


const char* BaseException::what() const throw() {
  return (message() + " from " + where() + ": " + why()).c_str();
}


BaseReasonedException::BaseReasonedException(const char* init_what,
                                             const char* file,
                                             const size_t line,
                                             const std::string& init_why) throw()
: BaseException(init_what, file, line)
, why_(init_why) { }


BaseReasonedException::~BaseReasonedException() throw () { }


const std::string BaseReasonedException::why() const {
  return (why_.size() != 0) ? why_ : std::string("Not specified");
}


GeneralException::GeneralException(const char* file, const size_t line, const std::string& init_why)
: BaseReasonedException("General error", file, line, init_why) { }


InvalidCast::InvalidCast(const char* file, const size_t line, const std::string& init_why)
: BaseReasonedException("Cast", file, line, init_why) { }


InvalidRange::InvalidRange(const char* file, const size_t line, const std::string& init_why)
: BaseReasonedException("Cast", file, line, init_why) { }


InvalidString::InvalidString(const char* file, const size_t line, const std::string& init_why)
: BaseReasonedException("InvalidString", file, line, init_why) { }


IOException::IOException(const char* file, const size_t line, const std::string& init_why)
: BaseReasonedException("I/O error", file, line, init_why) { }


InternalError::InternalError(const char* file, const size_t line, const std::string& init_why)
: BaseReasonedException("Internal error", file, line, init_why) { }


LogicError::LogicError(const char* file, const size_t line, const std::string& init_why)
: BaseReasonedException("Logic error", file, line, init_why) { }


TypeError::TypeError(const char* file, const size_t line, const std::string& init_why)
: BaseReasonedException("Type error", file, line, init_why) { }
