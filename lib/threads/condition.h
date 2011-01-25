// condition.h
// Threading library.
//
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef THREADS_CONDITION_H__
#define THREADS_CONDITION_H__

#include "mutex.h"
#include <base/prototype.h>
#include <clock/hiresclock.h>


namespace threads {

  class Condition : public base::NonCopyable {
  public:
    Condition();
    ~Condition();

    void Wait();
    bool Wait(const int msec);

    void Signal();
    void Broadcast();

    bool IsWaiting() const;

  private:
    pthread_cond_t condition_;
    Mutex mutex_;
    mutable Mutex lock_;
    clocks::HiResClock clock_;
    bool waiting_;
  };

} // namespace threads

#endif // THREADS_CONDITION_H__
