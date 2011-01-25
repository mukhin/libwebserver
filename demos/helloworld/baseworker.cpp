// baseworker.cpp
// Hello World Demo.
//
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "baseworker.h"

#ifdef ENABLE_PROFILING
# include <Saturn.h>
#endif

namespace demo {

  BaseWorker::BaseWorker() : logger_(log4cpp::Category::getRoot()) { }


  BaseWorker::~BaseWorker() { }


  void BaseWorker::Run() {
#ifdef ENABLE_PROFILING
    ::startSaturn();
#endif

    while (!ShouldStop()) {
      Process_();
    }

#ifdef ENABLE_PROFILING
    ::stopSaturn();
#endif
  }


  void BaseWorker::SetServer(const webserver::Server::sptr& server) {
    server_ = server;
  }


  const webserver::Server::sptr& BaseWorker::GetServer() const {
    return server_;
  }


} // namespace demo
