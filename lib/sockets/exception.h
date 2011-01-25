// exception.h
// Sockets library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#ifndef SOCKETS_EXCEPTION_H__
#define SOCKETS_EXCEPTION_H__

#include <base/exception.h>


namespace sockets {

  class SocketException : public BaseReasonedException {
  public:
    SocketException(const char* init_what, const char* file, const size_t line,
                    const std::string& init_why = "");
  };

  class ZeroReadBufferError : public SocketException {
  public:
    ZeroReadBufferError(const char* file, const size_t line,
                        const std::string& init_why = "Reading into buffer of zero size");
  };

  class ZeroWriteBufferError : public SocketException {
  public:
    ZeroWriteBufferError(const char* file, const size_t line,
                         const std::string& init_why = "Writing into buffer of zero size");
  };

  class ConnectionTerminatedError : public SocketException {
  public:
    ConnectionTerminatedError(const char* file, const size_t line,
                              const std::string& init_why = "Connection was terminated by remote peer");
  };

  class ConnectionTimeoutError : public SocketException {
  public:
    ConnectionTimeoutError(const char* file, const size_t line,
                           const std::string& init_why = "Connection timed out");
  };

  class ConnectionBlockedError : public SocketException {
  public:
    ConnectionBlockedError(const char* file, const size_t line,
                           const std::string& init_why = "Connection has been blocked permanently");
  };

  class UnknownSocketError : public SocketException {
  public:
    UnknownSocketError(const char* file, const size_t line,
                       const std::string& init_why = "");
  };

} // namespace sockets

#endif // SOCKETS_EXCEPTION_H__
