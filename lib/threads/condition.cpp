// condition.cpp
// Threading library.
//
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "condition.h"

extern "C" {
#include <pthread.h>
}


namespace threads {

  Condition::Condition()
  : waiting_(false) {
    ::pthread_cond_init(&condition_, 0);
  }


  Condition::~Condition() {
    Broadcast();
    if (::pthread_cond_destroy(&condition_) == EBUSY) {
      ::sched_yield();
    }
  }


  void Condition::Wait() {
    lock_.Lock();
    waiting_ = true;
    lock_.Unlock();

    ::pthread_cond_wait(&condition_, &mutex_.mutex_);

    lock_.Lock();
    waiting_ = false;
    lock_.Unlock();
  }


  bool Condition::Wait(const int msec) {
    int sec = msec / 1000;
    clock_.Advance(sec, (msec - sec * 1000) * 1000000);
    timespec time = clock_.ToTimespec();

    lock_.Lock();
    waiting_ = true;
    lock_.Unlock();

    int ret = ::pthread_cond_timedwait(&condition_, &mutex_.mutex_, &time);

    lock_.Lock();
    waiting_ = false;
    lock_.Unlock();

    if (ret != 0) {
      return false;
    }

    return true;
  }


  void Condition::Signal() {
    ::pthread_cond_signal(&condition_);
  }


  void Condition::Broadcast() {
    ::pthread_cond_broadcast(&condition_);
  }


  bool Condition::IsWaiting() const {
    lock_.Lock();
    const bool state = waiting_;
    lock_.Unlock();

    return state;
  }

} // namespace threads
