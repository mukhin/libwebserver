// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef WEBSERVER_BASE_CONNECTION_H__
#define WEBSERVER_BASE_CONNECTION_H__

#include "message.h"
#include <cstring/cstring.h>
#include <log4cpp/Category.hh>
#include <list>
#include <sockets/socket.h>
#include <tr1/memory>

namespace webserver {

  class Server;

  class BaseConnection : public sockets::Socket {
  public:
    typedef std::tr1::shared_ptr<BaseConnection> sptr;
    typedef std::tr1::weak_ptr<BaseConnection> wptr;
    typedef std::tr1::shared_ptr<Server> ServerSPtr;

    virtual ~BaseConnection();

    void EventRead();
    void EventWrite();
    void EventError();

    bool GetMessages(std::list<IncomingMessage::sptr>& messages);
    void SendMessage(const OutgoingMessage::sptr& message);

    void SetPersistence(const bool is_persistent);
    bool IsPersistent() const;

    void Initialize();
    void Close();

  protected:
    // Outgoing connection
    BaseConnection(const std::string& local, sockets::SocketAddress& remote, ServerSPtr& handler);
    // Incoming connection
    BaseConnection(const sockets::SocketFd& fd, ServerSPtr& handler);

    virtual void ProcessEventRead_(base::CString& buffer) = 0;
    virtual void AfterEventWrite_(const OutgoingMessage::sptr& message);
    void SetWeakThis_(const wptr& weak_this);
    void PushIncoming_(const IncomingMessage::sptr& incoming);
    void MarkActivity_();
    log4cpp::Category& GetLogger_();

  private:
    void SetOptions_();

    bool is_persistent_;
    const size_t buffer_length_;
    base::CString buffer_;
    ServerSPtr handler_;
    std::list<IncomingMessage::sptr> incoming_;
    std::list<OutgoingMessage::sptr> outgoing_;
    wptr weak_this_;
    log4cpp::Category& logger_;
  };

} // namespace webserver

#endif // WEBSERVER_BASE_CONNECTION_H__
