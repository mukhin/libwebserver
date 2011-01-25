// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef WEBSERVER_LISTENER_H__
#define WEBSERVER_LISTENER_H__

#include "baselistener.h"

namespace webserver {

  class HttpListener : public BaseListener {
  public:
    typedef threads::Thread<HttpListener> thread;

    HttpListener();
    ~HttpListener();

  private:
    void ProcessEventRead_(const sockets::SocketFd& fd);
  };

} // namespace webserver

#endif // WEBSERVER_LISTENER_H__
