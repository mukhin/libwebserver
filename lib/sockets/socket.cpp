// socket.cpp
// Sockets library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#include "socket.h"

#include "exception.h"
#include "socketfd.h"

extern "C" {
#include <string.h>
#include <sys/socket.h>
}

#include <base/basicmacros.h>
#include <base/c_format.h>
#include <cerrno>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PatternLayout.hh>

namespace sockets {

  Socket::Socket() {
    SetDescriptor(SocketFd());

#ifdef XENABLE_DEBUG
    log4cpp::Appender* appender = new log4cpp::FileAppender("SocketLogAppender", "/tmp/mp-socket.log");
    log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
    log4cpp::Category& logger = log4cpp::Category::getInstance(std::string("socket"));

    layout->setConversionPattern("%d{%d/%m/%Y %H:%M:%S,%l} : %c %p - %m%n");
    appender->setLayout(layout);
    logger.setAppender(appender);
    logger.setPriority(log4cpp::Priority::DEBUG);
#endif
  }

  Socket::~Socket() {
    if (Descriptor().IsValid()) {
      base_throw(IOException, c_format("~Socket() called, but descriptor %d is still valid", Descriptor().Descriptor()));
    }
  }

  // --- Socket options ---

  SocketAddress Socket::LocalAddress() const {
    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int result = ::getsockname(fd_, reinterpret_cast<sockaddr*>(&addr), &addrlen);

    ASSERT(addrlen == sizeof(addr));
    SocketAddress address;

    if (result >= 0) {
      address.FromSockAddr(addr);
    }
    else {
      ASSERT(result >= 0);
    }

    return address;
  }

  SocketAddress Socket::RemoteAddress() const {
    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int result = ::getpeername(fd_, reinterpret_cast<sockaddr*>(&addr), &addrlen);

    ASSERT(addrlen == sizeof(addr));
    SocketAddress address;

    if (result >= 0) {
      address.FromSockAddr(addr);
    }
    else {
      ASSERT(errno == ENOTCONN);
    }

    return address;
  }

  ssize_t Socket::ReadStream(char* buffer, const size_t length) {
    if (length == 0) {
      base_throw0(ZeroReadBufferError);
    }

    const ssize_t r = ::recv(fd_, buffer, length, 0);

    if (r == 0) {
      base_throw0(ConnectionTerminatedError);
    }
    else if (r < 0) {
      const int socket_errno = errno;
      if (socket_errno == EAGAIN || socket_errno == EINTR) {
        return 0;
      }
      else if (socket_errno == EWOULDBLOCK || socket_errno == ETIMEDOUT) {
        base_throw0(ConnectionTimeoutError);
      }
      else if (socket_errno == ECONNRESET || socket_errno == ECONNABORTED) {
        base_throw0(ConnectionTerminatedError);
      }
      else if (socket_errno == EDEADLK) {
        base_throw0(ConnectionBlockedError);
      }
      else {
        base_throw(UnknownSocketError, c_format("Connection error %d: %s", socket_errno, ::strerror(socket_errno)));
      }
    }

    return r;
  }

  ssize_t Socket::ReadStreamFrom(SocketAddress* paddr, char* buffer, const size_t length) {
    if (length == 0) {
      base_throw0(ZeroReadBufferError);
    }

    sockaddr_in saddr;
    socklen_t cbAddr = sizeof(saddr);

    if ((paddr != 0)) {
      paddr->FromSockAddr(saddr);
    }

    return ::recvfrom(fd_, buffer, length, 0, reinterpret_cast<sockaddr*>(&saddr), &cbAddr);
  }

  ssize_t Socket::WriteStream(const void* buffer, const size_t length) {
    if (length == 0) {
      base_throw0(ZeroWriteBufferError);
    }

    Descriptor().SetSendBufferSize(static_cast<uint32_t>(length));
    const ssize_t r = ::send(fd_, buffer, length, 0);

#ifdef XENABLE_DEBUG
    log4cpp::Category& logger = log4cpp::Category::getInstance(std::string("socket"));
    logger.debugStream() << "Wrote " << r << " bytes when sending buffer of " << length << " bytes: <buffer>" <<
    static_cast<const char*>(buffer) << "</buffer>";
#endif

    if (r == 0) {
      base_throw0(ConnectionTerminatedError);
    }
    else if (r < 0) {
      const int socket_errno = errno;
      if (socket_errno == EAGAIN || socket_errno == EINTR) {
        return 0;
      }
      else if (socket_errno == EWOULDBLOCK || socket_errno == ETIMEDOUT) {
        base_throw0(ConnectionTimeoutError);
      }
      else if (socket_errno == ECONNRESET || socket_errno == ECONNABORTED) {
        base_throw0(ConnectionTerminatedError);
      }
      else if (socket_errno == EDEADLK) {
        base_throw0(ConnectionBlockedError);
      }
      else {
        base_throw(UnknownSocketError, c_format("Connection error %d: %s", socket_errno, ::strerror(socket_errno)));
      }
    }

    return r;
  }

  ssize_t Socket::WriteStreamTo(const SocketAddress& addr, const void* buffer, const size_t length) {
    if (length == 0) {
      base_throw0(ZeroWriteBufferError);
    }

    sockaddr_in saddr;
    addr.ToSockAddr(&saddr);
    return ::sendto(fd_, buffer, length, 0, reinterpret_cast<sockaddr*>(&saddr), sizeof(saddr));
  }

} // namespace sockets
