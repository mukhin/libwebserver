// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "httpconnection.h"
#include "exception.h"
#include "incominghttpmessage.h"
#include "status.h"

namespace webserver {

  HttpConnection::HttpConnection(const std::string& local, sockets::SocketAddress& remote, Server::sptr& handler)
  : BaseConnection(local, remote, handler)
  , buffer_has_bad_data_(false) { }


  HttpConnection::HttpConnection(const sockets::SocketFd& fd, Server::sptr& handler)
  : BaseConnection(fd, handler)
  , buffer_has_bad_data_(false) { }


  HttpConnection::sptr HttpConnection::Create(const std::string& local, const std::string& remote, const uint16_t port,
                                              bool is_persistent, Server::sptr& handler) {
    sockets::SocketAddress addr(remote, port);
    HttpConnection::sptr c = HttpConnection::sptr(new HttpConnection(local, addr, handler));
    c->SetPersistence(is_persistent);
    c->SetWeakThis_(c);
    handler->NewConnection(c);
    c->Initialize();
    return c;
  }


  HttpConnection::sptr HttpConnection::Create(const sockets::SocketFd& fd, Server::sptr& handler) {
    HttpConnection::sptr c = HttpConnection::sptr(new HttpConnection(fd, handler));
    c->SetWeakThis_(c);

    try {
      handler->NewConnection(c);
      c->Initialize();
    }
    catch (const TooManyConnectionsError& e) {
      c->SendMessage(OutgoingHttpMessage::TooManyConnections());
    }

    return c;
  }


  void HttpConnection::ProcessEventRead_(base::CString& buffer) {
    IncomingHttpMessage::sptr message;
    size_t eof = 0;

    try {
      while (IncomingHttpMessage::Deserialize(message, buffer, eof)) {
        PushIncoming_(message);
        buffer.Erase(0, eof);
        SetPersistence(message->IsPersistent());
        MarkActivity_();
        Status::Self()->IncomingRequest(eof);
        eof = 0;
      }

      buffer_has_bad_data_ = false;
    }
    catch (const DeserializationError& e) {
      if (buffer_has_bad_data_) {
        IncomingHttpMessage::HttpPair id;
        if (message->GetRequestId(id)) {
          SendMessage(OutgoingHttpMessage::BadRequest(id->value));
        }
        else {
          SendMessage(OutgoingHttpMessage::BadRequest());
        }

        GetLogger_().warnStream() << "HTTP deserialization failed: " + e.why() + "\n<buffer>" + buffer.Str() + "</buffer>";
      } else {
        buffer_has_bad_data_ = true;
      }
    }
  }


  void HttpConnection::AfterEventWrite_(const OutgoingMessage::sptr& message) {
    Status::Self()->ServedRequest(std::tr1::dynamic_pointer_cast<OutgoingHttpMessage>(message));
  }

} // namespace webserver

