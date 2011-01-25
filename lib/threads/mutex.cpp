// mutex.cpp
// Threading library.
//
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "mutex.h"
#include <cerrno>


namespace threads {

  Mutex::Mutex() {
    ::pthread_mutex_init(&mutex_, 0);
  }


  Mutex::~Mutex() {
    if (::pthread_mutex_destroy(&mutex_) == EBUSY) {
      ::sched_yield();
    }
  }


  void Mutex::Lock() {
    ::pthread_mutex_lock(&mutex_);
  }


  bool Mutex::TryLock() {
    int ret = ::pthread_mutex_trylock(&mutex_);

    if (ret != 0) {
      return false;
    }

    return true;
  }


  void Mutex::Unlock() {
    ::pthread_mutex_unlock(&mutex_);
  }

} // namespace threads
