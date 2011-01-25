// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef WEBSERVER_BASE_LISTENER_H__
#define WEBSERVER_BASE_LISTENER_H__

#include <base/basicmacros.h>
#include <io/poll.h>
#include <sockets/socket.h>
#include <tbb/spin_mutex.h>
#include <threads/thread.h>
#include <tr1/memory>
#include <vector>


namespace webserver {

  class Server;

  class BaseListener : public sockets::Socket, public threads::Worker {
  public:
    typedef threads::Thread<BaseListener> thread;
    typedef std::tr1::shared_ptr<Server> ServerSPtr;

    BaseListener();
    virtual ~BaseListener();

    void AddHandler(const std::tr1::shared_ptr<Server>& server);
    ServerSPtr& GetCurrentHandler();

    bool Open(const std::string& host, const uint16_t port, const unsigned int backlog);
    void Close();

    void EventRead();
    void EventWrite() __attribute__((noreturn));
    void EventError() __attribute__((noreturn));

    void Run();

  protected:
    virtual void ProcessEventRead_(const sockets::SocketFd& fd) = 0;

  private:
    typedef std::vector<ServerSPtr> HandlerPool;
    io::Poll* poll_;
    HandlerPool handlers_;
    HandlerPool::size_type current_handler_;
    tbb::spin_mutex mutex_;
  };

} // namespace webserver

#endif // WEBSERVER_BASE_LISTENER_H__
