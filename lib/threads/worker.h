// worker.h
// Threading library.
//
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef THREADS_WORKER_H__
#define THREADS_WORKER_H__

#include "condition.h"
#include <base/prototype.h>


namespace threads {

  class Worker : public base::NonCopyable {
  public:
    Worker();
    virtual ~Worker();

    virtual void Run() = 0;
    void Stop();

    void Yield();
    void Wait();
    void Wait(const int msec);
    void Continue();
    void Broadcast();

    bool IsActive() const;
    bool ShouldStop() const;
    void SetStoppedStatus();
    void ClearStoppedStatus();

  protected:
    Condition idle_;
    bool should_stop_;
    bool is_stopped_;
    bool start_requested_;
  };

} // namespace threads

#endif // THREADS_WORKER_H__
