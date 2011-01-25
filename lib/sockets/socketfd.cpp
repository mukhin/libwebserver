// socketfd.cpp
// Sockets library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "socketfd.h"

namespace sockets {

  int SocketFd::GetError() const {
    CheckValidity_();

    int err;
    socklen_t length = sizeof(err);

    if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &err, &length) == -1) {
      base_throw(IOException, "Could not get error.");
    }

    return err;
  }

  bool SocketFd::Connect(const SocketAddress& addr) {
    CheckValidity_();

    SocketAddress addr2(addr);
    if (addr2.IsUnresolved()) {
      if(!addr2.Resolve()) {
        base_throw(IOException, "Cannot resolve address");
      }
    }

    sockaddr_in saddr;
    addr2.ToSockAddr(&saddr);
    return ::connect(fd_, reinterpret_cast<sockaddr*>(&saddr), sizeof(saddr)) == 0;
  }

  SocketFd SocketFd::Accept(SocketAddress* paddr) {
    CheckValidity_();

    sockaddr_in saddr;
    socklen_t cbAddr = sizeof(saddr);

    if (paddr != 0) {
      paddr->FromSockAddr(saddr);
    }

    return SocketFd(::accept(fd_, reinterpret_cast<sockaddr*>(&saddr), &cbAddr));
  }

  void SocketFd::CheckValidity_() const {
    if (!IsValid()) {
      base_throw(IOException, "Socket function called on an invalid fd.");
    }
  }

} // namespace sockets
