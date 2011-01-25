// baseworker.h
// Hello World Demo.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef DEMO_BASE_MEDIAWORKER_H__
#define DEMO_BASE_MEDIAWORKER_H__

#include <log4cpp/Category.hh>
#include <threads/thread.h>
#include <webserver/server.h>


namespace demo {

  class BaseWorker : public threads::Worker {
  public:
    BaseWorker();
    virtual ~BaseWorker();

    void Run();
    void SetServer(const webserver::Server::sptr& server);
    const webserver::Server::sptr& GetServer() const;

  protected:
    virtual void Process_() = 0;

    webserver::Server::sptr server_;
    log4cpp::Category& logger_;
  };

} // namespace demo

#endif // DEMO_BASE_MEDIAWORKER_H__
