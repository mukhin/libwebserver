// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef WEBSERVER_HTTP_CONNECTION_H__
#define WEBSERVER_HTTP_CONNECTION_H__

#include "baseconnection.h"
#include "outgoinghttpmessage.h"
#include "server.h"

namespace webserver {

  class HttpConnection : public BaseConnection {
  public:
    typedef std::tr1::shared_ptr<HttpConnection> sptr;
    typedef std::tr1::weak_ptr<HttpConnection> wptr;

    // Outgoing connection
    static sptr Create(const std::string& local, const std::string& remote, const uint16_t port,
                       bool is_persistent, Server::sptr& handler);

    // Incoming connection
    static sptr Create(const sockets::SocketFd& fd, Server::sptr& handler);

  private:
    // Outgoing connection
    HttpConnection(const std::string& local, sockets::SocketAddress& remote, Server::sptr& handler);
    // Incoming connection
    HttpConnection(const sockets::SocketFd& fd, Server::sptr& handler);

    void ProcessEventRead_(base::CString& buffer);
    void AfterEventWrite_(const OutgoingMessage::sptr& message);

    bool buffer_has_bad_data_;
  };

} // namespace webserver

#endif // WEBSERVER_HTTP_CONNECTION_H__
