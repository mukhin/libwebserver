// exception.cpp
// Sockets library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#include "exception.h"


namespace sockets {
  SocketException::SocketException(const char* init_what, const char* file, const size_t line,
                                   const std::string& init_why)
  : BaseReasonedException(init_what, file, line, init_why) { }

  ZeroReadBufferError::ZeroReadBufferError(const char* file, const size_t line,
                                           const std::string& init_why)
  : SocketException("Zero read buffer.", file, line, init_why) { }

  ZeroWriteBufferError::ZeroWriteBufferError(const char* file, const size_t line,
                                             const std::string& init_why)
  : SocketException("Zero write buffer.", file, line, init_why) { }

  ConnectionTerminatedError::ConnectionTerminatedError(const char* file, const size_t line,
                                                       const std::string& init_why)
  : SocketException("Socket error.", file, line, init_why) { }

  ConnectionTimeoutError::ConnectionTimeoutError(const char* file, const size_t line,
                                                 const std::string& init_why)
  : SocketException("Socket error.", file, line, init_why) { }

  ConnectionBlockedError::ConnectionBlockedError(const char* file, const size_t line,
                                                 const std::string& init_why)
  : SocketException("Socket error.", file, line, init_why) { }

  UnknownSocketError::UnknownSocketError(const char* file, const size_t line,
                                         const std::string& init_why)
  : SocketException("Unknown socket error.", file, line, init_why) { }

} // namespace sockets
