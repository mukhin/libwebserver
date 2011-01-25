// mutex.h
// Threading library.
//
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef THREADS_MUTEX_H__
#define THREADS_MUTEX_H__

#include <base/prototype.h>

extern "C" {
#include <pthread.h>
}


namespace threads {

  class Condition;

  class Mutex : public base::NonCopyable {
  public:
    Mutex();
    ~Mutex();

    void Lock();
    bool TryLock();
    void Unlock();

  private:
    friend class Condition;
    pthread_mutex_t mutex_;
  };

} // namespace threads

#endif // THREADS_MUTEX_H__
