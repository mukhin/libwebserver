// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef WEBSERVER_SERVER_H__
#define WEBSERVER_SERVER_H__

#include "baseconnection.h"
#include "baselistener.h"
#include "httptypes.h"

#include <clock/clock.h>
#include <inttypes.h>
#include <io/io.h>
#include <set>
#include <tbb/atomic.h>
#include <tbb/spin_mutex.h>
#include <threads/thread.h>
#include <tr1/memory>


namespace webserver {

  class ConnectionLess {
  public:
    bool operator()(const BaseConnection::sptr& a, const BaseConnection::sptr& b) const {
      return a->Descriptor().Descriptor() < b->Descriptor().Descriptor();
    }
  };


  class Server : public base::NonCopyable {
  public:
    typedef std::tr1::shared_ptr<Server> sptr;

    ~Server();

    static sptr Create();
    template<class T>
    static sptr Create(std::tr1::shared_ptr<threads::Thread<T> >& listener);

    io::Poll* GetPoll() const;

    static void SetMaxConnects(const unsigned int max_connects);
    static void SetConnectionTimeout(const unsigned int timeout);

    // Create listening non-blocking socket bound to host:port with timeout in milliseconds.
    template<class T>
    static std::tr1::shared_ptr<threads::Thread<T> > CreateListener(const std::string& host, const uint16_t port, const unsigned int backlog);

    void NewConnection(const BaseConnection::sptr& c);
    void DeleteConnection(const BaseConnection::wptr& c);

    template<class T>
    bool GetActiveConnection(std::tr1::shared_ptr<T>& c);
    void ActivityOn(const BaseConnection::wptr& c);
    unsigned int ActiveConnections() const;
    unsigned int GetConnectionTimeout() const;

    void Perform();

  private:
    Server();
    Server(std::tr1::shared_ptr<threads::Thread<BaseListener> >& listener);

    void Constructor_();

    static unsigned int max_connects_;
    static unsigned int connection_timeout_;
    static tbb::atomic<unsigned int> servers_count_;

    typedef std::set<BaseConnection::sptr, ConnectionLess> ConnectionSet;
    ConnectionSet connections_;
    ConnectionSet activity_;

    io::Poll* poll_;
    std::tr1::shared_ptr<threads::Thread<BaseListener> > listener_;
    tbb::spin_mutex mutex_;
  };


  template<class T>
  Server::sptr Server::Create(std::tr1::shared_ptr<threads::Thread<T> >& listener) {
    Server::sptr s = Server::sptr(new Server());
    listener->AddHandler(s);
    return s;
  }


  template<class T>
  std::tr1::shared_ptr<threads::Thread<T> > Server::CreateListener(const std::string& host, const uint16_t port, const unsigned int backlog) {
    if (backlog >= max_connects_) {
      base_throw(InternalError, "Backlog cannot exceed maximal connections number.");
    }

    unsigned int tries = 0;
    std::tr1::shared_ptr<threads::Thread<T> > listener = std::tr1::shared_ptr<threads::Thread<T> >(new threads::Thread<T>(false));

    while(true) {
      if (!listener->Open(host, port, backlog)) {
        if (tries++ < 3) {
          clocks::Clock::Sleep(1);
        }
        else {
          base_throw(IOException, "Cannot start listener.");
        }
      }
      else {
        break;
      }
    }

    listener->Start();
    return listener;
  }


  template<class T>
  bool Server::GetActiveConnection(std::tr1::shared_ptr<T>& c) {
    if (activity_.empty()) {
      return false;
    }

    ConnectionSet::iterator it = activity_.begin();
    c = std::tr1::dynamic_pointer_cast<T>(*it);
    activity_.erase(it);
    return true;
  }

} // namespace webserver

#endif // WEBSERVER_SERVER_H__
