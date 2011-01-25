// scheduler.h
// Hello World Demo.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef DEMO_SCHEDULER_H__
#define DEMO_SCHEDULER_H__

#include "worker.h"

#include <base/basicmacros.h>
#include <base/prototype.h>
#include <list>
#include <threads/singleton.h>
#include <threads/thread.h>
#include <tr1/memory>
#include <webserver/listener.h>


namespace demo {

  class _Scheduler : public threads::Worker {
  public:
    void Setup();
    void Run();

    bool ArePoolsActive() const;
    unsigned int ActiveHttpConnections() const;


  protected:
    _Scheduler();
    ~_Scheduler();

  private:
    void Shutdown_();

    void StartWorkers_();
    void StopWorkers_();

    // Workers pool.
    typedef std::list<std::tr1::shared_ptr<threads::Thread<HttpWorker> > > HttpPool;
    HttpPool http_pool_;

    std::tr1::shared_ptr<threads::Thread<webserver::HttpListener> > http_listener_;
  };

  typedef threads::Singleton<_Scheduler> Scheduler;

} // namespace demo

#endif // DEMO_SCHEDULER_H__
