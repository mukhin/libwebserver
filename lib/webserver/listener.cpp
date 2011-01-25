// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "listener.h"
#include "httpconnection.h"

namespace webserver {

  HttpListener::HttpListener() { }
  HttpListener::~HttpListener() { }


  void HttpListener::ProcessEventRead_(const sockets::SocketFd& fd) {
    HttpConnection::Create(fd, GetCurrentHandler());
  }


} // namespace webserver
