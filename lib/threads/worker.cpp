// worker.cpp
// Threading library.
//
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "worker.h"

namespace threads {

  Worker::Worker()
  : should_stop_(false)
  , is_stopped_(true)
  , start_requested_(false) { }


  Worker::~Worker() { }


  void Worker::Stop() {
    should_stop_ = true;
    Broadcast();
  }


  void Worker::Yield() {
    ::sched_yield();
  }

  void Worker::Wait() {
    idle_.Wait();
  }


  void Worker::Wait(const int msec) {
    idle_.Wait(msec);
  }


  void Worker::Continue() {
    idle_.Signal();
  }


  void Worker::Broadcast() {
    idle_.Broadcast();
  }


  bool Worker::IsActive() const {
    return !is_stopped_;
  }


  bool Worker::ShouldStop() const {
    return should_stop_;
  }


  void Worker::SetStoppedStatus() {
    is_stopped_ = true;
    start_requested_ = false;
  }


  void Worker::ClearStoppedStatus() {
    should_stop_ = false;
    is_stopped_ = false;
  }

} // namespace threads
