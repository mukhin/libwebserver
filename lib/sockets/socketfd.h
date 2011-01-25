// socketfd.h
// Sockets library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef SOCKETS_SOCKETFD_H__
#define SOCKETS_SOCKETFD_H__

#include "socketaddress.h"

#include <inttypes.h>
#include <base/exception.h>
#include <cerrno>

extern "C" {
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
}


namespace sockets {

  // @interface

  class SocketFd {
  public:
    SocketFd() : fd_(-1) { }
    explicit SocketFd(const int fd) : fd_(fd) { }

    int Descriptor() const;
    void SetDescriptor(const int fd);
    bool IsValid() const;

    // Sets socket to blocking or non-blocking operation mode.
    bool SetBlocking();
    bool SetNonBlocking();

    // Set socket receive and send timeouts.
    bool SetSendTimeout(const unsigned long usec);
    bool SetReceiveTimeout(const unsigned long usec);

    // Enables the periodic transmission of messages on a connected socket to detect broken connections.
    bool SetKeepAlive(bool state);
    // Allows (true) or denies (false) to reuse socket address.
    bool SetReuseAddress(bool state);
    // Sets socket send buffer size.
    bool SetSendBufferSize(const uint32_t size);
    // Sets socket receive buffer size.
    bool SetReceiveBufferSize(const uint32_t size);
    // Gets socket send buffer size.
    unsigned int GetSendBufferSize() const;
    // Gets socket receive buffer size.
    unsigned int GetReceiveBufferSize() const;
    // Whether socket errors should return EPIPE instead of generating SIGPIPE.
    bool SetNoSignal(bool state);
    // Enables or disables Nagle algorithm.
    bool SetNoDelay(bool state);
    // Enables or disables sending partial frames on Linux.
    bool SetNoPartialFrames(bool state);

    // Get and clear error on the socket.
    int GetError() const;

    // Creates TCP stream socket.
    bool Open();
    // Closes socket and deletes descriptor.
    void Close();
    // Binds socket to address.
    bool Bind(const SocketAddress& addr);
    // Connects socket to endpoint.
    bool Connect(const SocketAddress& addr);
    // Listens for connections.
    bool Listen(const int backlog);
    // Accepts connection and creates new socket.
    SocketFd Accept(SocketAddress* paddr = 0);

  private:
    void CheckValidity_() const;

    int fd_;
  };



  // @implementation

  inline int SocketFd::Descriptor() const {
    return fd_;
  }

  inline void SocketFd::SetDescriptor(const int fd) {
    fd_ = fd;
  }

  inline bool SocketFd::IsValid() const {
    return fd_ >= 0;
  }

  inline bool SocketFd::SetBlocking() {
    CheckValidity_();

    int flags;
    if ((flags = ::fcntl(fd_, F_GETFL, 0)) == -1) {
      base_throw(IOException, "Cannot get socket flags.");
      return false;
    }

    return ::fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK) == 0;
  }

  inline bool SocketFd::SetNonBlocking() {
    CheckValidity_();

    int flags;
    if ((flags = ::fcntl(fd_, F_GETFL, 0)) == -1) {
      base_throw(IOException, "Cannot get socket flags.");
      return false;
    }

    return ::fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == 0;
  }

  inline bool SocketFd::SetSendTimeout(const unsigned long usec) {
    CheckValidity_();
    timeval tv = { usec / 1000000, usec % 1000000 };
    return ::setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == 0;
  }

  inline bool SocketFd::SetReceiveTimeout(const unsigned long usec) {
    CheckValidity_();
    timeval tv = { usec / 1000000, usec % 1000000 };
    return ::setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == 0;
  }

  inline bool SocketFd::SetKeepAlive(bool state) {
    CheckValidity_();

    int opt = state ? 1 : 0;
    return ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) == 0;
  }

  inline bool SocketFd::SetReuseAddress(bool state) {
    CheckValidity_();

    int opt = state ? 1 : 0;
    return ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == 0;
  }

  inline bool SocketFd::SetSendBufferSize(uint32_t size) {
    CheckValidity_();

    uint32_t opt = size;
    return ::setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, &opt, sizeof(opt)) == 0;
  }

  inline bool SocketFd::SetReceiveBufferSize(uint32_t size) {
    CheckValidity_();

    uint32_t opt = size;
    return ::setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, &opt, sizeof(opt)) == 0;
  }

  inline unsigned int SocketFd::GetSendBufferSize() const {
    CheckValidity_();

    int opt;
    socklen_t len = sizeof(opt);
    if (::getsockopt(fd_, SOL_SOCKET, SO_SNDBUF, &opt, &len) == 0) {
      return opt;
    }
    return 0;
  }

  inline unsigned int SocketFd::GetReceiveBufferSize() const {
    CheckValidity_();

    int opt;
    socklen_t len = sizeof(opt);
    if (::getsockopt(fd_, SOL_SOCKET, SO_RCVBUF, &opt, &len) == 0) {
      return opt;
    }
    return 0;
  }

  inline bool SocketFd::SetNoSignal(bool state) {
#ifdef OS_MACOSX
    int opt;
    if (state) {
      opt = 1;
    }
    else {
      opt = 0;
    }

    return ::setsockopt(fd_, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) == 0;
#else
    static_cast<void>(state);
    return true;
#endif
  }

  inline bool SocketFd::SetNoDelay(bool state) {
    int opt;
    if (state) {
      opt = 1;
    }
    else {
      opt = 0;
    }

    return ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) == 0;
  }

  inline bool SocketFd::SetNoPartialFrames(bool state) {
    int opt;
    if (state) {
      opt = 1;
    }
    else {
      opt = 0;
    }
#ifdef OS_LINUX
    return ::setsockopt(fd_, IPPROTO_TCP, TCP_CORK, &opt, sizeof(opt)) == 0;
#elif defined OS_MACOSX
    return ::setsockopt(fd_, IPPROTO_TCP, TCP_NOPUSH, &opt, sizeof(opt)) == 0;
#endif
  }

  inline bool SocketFd::Open() {
    return (fd_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != -1;
  }

  inline void SocketFd::Close() {
    const int _fd = fd_;
    fd_ = -1;
    ::close(_fd);
  }

  inline bool SocketFd::Bind(const SocketAddress& addr) {
    CheckValidity_();

    sockaddr_in saddr;
    addr.ToSockAddr(&saddr);
    return ::bind(fd_, reinterpret_cast<sockaddr*>(&saddr), sizeof(saddr)) == 0;
  }

  inline bool SocketFd::Listen(const int backlog) {
    CheckValidity_();
    return ::listen(fd_, backlog) == 0;
  }

} // namespace sockets

#endif // SOCKETS_SOCKETFD_H__
