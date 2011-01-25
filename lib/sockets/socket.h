// socket.h
// Sockets library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#ifndef SOCKETS_SOCKET_H__
#define SOCKETS_SOCKET_H__

#include "socketaddress.h"
#include "socketfd.h"

#include <base/istring.h>
#include <io/event.h>
#include <string>
#include <tr1/memory>

extern "C" {
#include <sys/types.h>
#include <netinet/in.h>
}

using std::tr1::shared_ptr;


namespace sockets {

  // @interface

  class Socket : public io::Event {
  public:
    Socket();
    ~Socket();

    SocketAddress LocalAddress() const;
    SocketAddress RemoteAddress() const;

    SocketFd& Descriptor();
    const SocketFd& Descriptor() const;
    void SetDescriptor(const SocketFd fd);

    // Reads from socket and returns amount of data read.
    ssize_t ReadStream(char* buffer, const size_t length);
    // Reads from specified address and returns amount of data read.
    ssize_t ReadStreamFrom(SocketAddress* paddr, char* buffer, const size_t length);

    // Writes into socket and returns amount of data written.
    ssize_t WriteStream(const void* buffer, const size_t length);
    // Writes to specified address and returns amount of data written.
    ssize_t WriteStreamTo(const SocketAddress& addr, const void* buffer, const size_t length);
  };



  // @implementation

  inline SocketFd& Socket::Descriptor() {
    return *reinterpret_cast<SocketFd*>(&fd_);
  }

  inline const SocketFd& Socket::Descriptor() const {
    return *reinterpret_cast<const SocketFd*>(&fd_);
  }

  inline void Socket::SetDescriptor(const SocketFd fd) {
    fd_ = fd.Descriptor();
  }

} // namespace sockets

#endif // SOCKETS_SOCKET_H__
