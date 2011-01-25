// thread.h
// Threading library.
//
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef THREADS_THREAD_H__
#define THREADS_THREAD_H__

#include "condition.h"
#include "exception.h"
#include "worker.h"
#include <base/basicmacros.h>
#include <base/prototype.h>

extern "C" {
#include <pthread.h>
}


namespace threads {

  static inline void Yield() {
    ::sched_yield();
  }


  template<class __Worker>
  class Thread : public __Worker {
  public:
    Thread(bool autostart = false, bool detachable = false);
    ~Thread();

    void Start();
    void Stop();

    void Join();
    void Detach();

    bool IsActive() const;

  protected:
    static void* Wrapper_(void* ptr_this) __attribute__((noreturn));
    pthread_t tid_;

  private:
    const bool detached_;
  };

} // namespace threads

#include "thread.hpp"

#endif // THREADS_THREAD_H__
