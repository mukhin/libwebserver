// worker.h
// Hello World Demo.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef DEMO_WORKER_H__
#define DEMO_WORKER_H__

#include "baseworker.h"
#include <webserver/server.h>
#include <webserver/listener.h>
#include <webserver/incominghttpmessage.h>
#include <webserver/outgoinghttpmessage.h>


namespace demo {

  class HttpWorker : public BaseWorker {
  public:
    static std::tr1::shared_ptr<threads::Thread<HttpWorker> > Create(std::tr1::shared_ptr<threads::Thread<webserver::HttpListener> >& listener);
  protected:
    HttpWorker();
    ~HttpWorker();
    void Process_();
  };

} // namespace demo

#endif // DEMO_WORKER_H__
