// scheduler.cpp
// Hello World Demo.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "scheduler.h"

#include <clock/clock.h>
#include <log4cpp/Category.hh>
#include <webserver/server.h>
#include <webserver/incominghttpmessage.h>
#include <webserver/outgoinghttpmessage.h>


namespace demo {

  _Scheduler::_Scheduler() { }
  _Scheduler::~_Scheduler() { }


  void _Scheduler::Run() {
    while (!ShouldStop()) {
      clocks::Clock::Tick();

      // Run periodic tasks
      // taskmanager_.Execute();

      Wait(500);
    }

    Shutdown_();
  }


  bool _Scheduler::ArePoolsActive() const {
    for (HttpPool::const_iterator i = http_pool_.begin(); i != http_pool_.end(); ++i) {
      if (*i && (*i)->IsActive()) {
        return true;
      }
    }

    return false;
  }


  unsigned int _Scheduler::ActiveHttpConnections() const {
    unsigned int count = 0;
    for (HttpPool::const_iterator i = http_pool_.begin(); i != http_pool_.end(); ++i) {
      count += (**i).GetServer()->ActiveConnections();
    }
    return count;
  }


  void _Scheduler::Setup() {
    log4cpp::Category& logger = log4cpp::Category::getRoot();
    //Status::Init(); // Runtime counters

    logger.info("Starting initialization sequence...");

    clocks::HiResTimer timer;

    StartWorkers_();

    logger.info("Initialization complete, ready to rumble!");
  }


  void _Scheduler::Shutdown_() {
    // Stop and destroy threads.
    StopWorkers_();

    while (ArePoolsActive()) {
      clocks::Clock::Sleep(1);
    }
  }


  // All those parameters were in config file before.
  // We exctracted them here just to illustrate the way we used to configurate webserver::Server.
  // We have a Configuration class to read and collect all parameters together.
  #define DEMO_MAX_CONNECTS 1000
  #define DEMO_CONNECTION_TIMEOUT 10
  #define DEMO_BACKLOG_SIZE 100
  #define DEMO_WORKERS_COUNT 10
  
  #define DEMO_HOSTNAME "localhost"
  #define DEMO_PORT 9000

  void _Scheduler::StartWorkers_() {
    webserver::Server::SetMaxConnects(DEMO_MAX_CONNECTS);
    webserver::Server::SetConnectionTimeout(DEMO_CONNECTION_TIMEOUT);

    log4cpp::Category& logger = log4cpp::Category::getRoot();
    logger.info("Starting processing workers.");

    http_listener_ = webserver::Server::CreateListener<webserver::HttpListener>(DEMO_HOSTNAME, DEMO_PORT, DEMO_BACKLOG_SIZE);
    for (unsigned int i = 0; i < DEMO_WORKERS_COUNT; ++i) {
      std::tr1::shared_ptr<threads::Thread<HttpWorker> > worker = HttpWorker::Create(http_listener_);
      http_pool_.push_back(worker);
    }
  }


  void _Scheduler::StopWorkers_() {
    log4cpp::Category& logger = log4cpp::Category::getRoot();
    logger.info("Stopping processing workers.");

    for (HttpPool::const_iterator i = http_pool_.begin(); i != http_pool_.end(); ++i) {
      (**i).Stop();
      (**i).Join();
    }
  }

} // namespace demo
