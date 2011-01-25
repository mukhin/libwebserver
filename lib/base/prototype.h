// prototype.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef BASE_PROTOTYPE_H__
#define BASE_PROTOTYPE_H__

#include "common.h"

namespace base {

  class NonCopyable {
  protected:
    NonCopyable() { }
    ~NonCopyable() { }

  private:
    NonCopyable(const NonCopyable&);
    const NonCopyable& operator=(const NonCopyable&);
  };


  template<class T>
  class Singleton : public NonCopyable, public T {
  public:
    inline static T* Self() {
      return self_;
    }

    inline static void Init() {
      self_ = new T();
    }

    inline static void Destroy() {
      destroy(self_);
    }

  protected:
    Singleton() { }
    ~Singleton() { }

  private:
    static T* self_;
  };

  template<class T> T* Singleton<T>::self_ = 0;

} // namespace base

#endif // BASE_PROTOTYPE_H__
