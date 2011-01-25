// singleton.h
// Threading library.
//
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef THREADS_SINGLETON_H__
#define THREADS_SINGLETON_H__

#include <base/prototype.h>
#include "thread.h"


namespace threads {

  template<class T>
  class Singleton : public Thread<T> {
  public:
    static Thread<T>* Self() {
      return self_;
    }

    static void Init(bool autostart = false, bool detached = false) {
      self_ = new Thread<T>(autostart, detached);
    }

    static void Destroy() {
      destroy(self_);
    }

  protected:
    Singleton() { }
    ~Singleton() { }

  private:
    static Thread<T>* self_;
  };

  template<class T> Thread<T>* Singleton<T>::self_;

} // namespace threads

#endif // THREADS_SINGLETON_H__
