// thread.hpp
// Threading library.
//
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef THREADS_THREAD_HPP__
#define THREADS_THREAD_HPP__

namespace threads {

  template<class __Worker>
  Thread<__Worker>::Thread(bool autostart, bool detached)
  : detached_(detached) {
    LOG("Creating thread " << __PRETTY_FUNCTION__);
    if (autostart) {
      Start();
    }
  }

  template<class __Worker>
  Thread<__Worker>::~Thread() {
    Stop();
    LOG("Destroying thread " << __PRETTY_FUNCTION__);
  }


  template<class __Worker>
  void Thread<__Worker>::Start() {
    __Worker::start_requested_ = true;
    pthread_attr_t attr;
    int ret = ::pthread_attr_init(&attr);
    if (ret != 0) {
      base_throw(ThreadException, ::strerror(ret));
    }

    ret = ::pthread_attr_setdetachstate(&attr, detached_ ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE);
    if (ret != 0) {
      base_throw(ThreadException, ::strerror(ret));
    }

    ret = ::pthread_create(&tid_, &attr, Wrapper_, this);
    if (ret != 0) {
      base_throw(ThreadException, ::strerror(ret));
    }

    ret = ::pthread_attr_destroy(&attr);
    if (ret != 0) {
      base_throw(ThreadException, ::strerror(ret));
    }
  }

  template<class __Worker>
  void Thread<__Worker>::Stop() {
    __Worker::Stop();
  }


  template<class __Worker>
  void Thread<__Worker>::Join() {
    if (detached_) {
      LOG("Not joining " << __PRETTY_FUNCTION__);
      return;
    }

    if (__Worker::start_requested_) {
      while (!IsActive()) {
        Yield();
      }
    }
    else {
      LOG("Not joining " << __PRETTY_FUNCTION__);
      return;
    }

    LOG("Joining " << __PRETTY_FUNCTION__);

    int ret = ::pthread_join(tid_, 0);
    if (ret != 0) {
      base_throw(ThreadException, ::strerror(ret));
    }
  }


  template<class __Worker>
  void Thread<__Worker>::Detach() {
    if (detached_) {
      LOG("Not detaching " << __PRETTY_FUNCTION__);
      return;
    }

    if (__Worker::start_requested_) {
      while (!IsActive()) {
        Yield();
      }
    }
    else {
      LOG("Not detaching " << __PRETTY_FUNCTION__);
      return;
    }

    LOG("Detaching " << __PRETTY_FUNCTION__);

    int ret = ::pthread_detach(tid_);
    if (ret != 0) {
      base_throw(ThreadException, ::strerror(ret));
    }
  }


  template<class __Worker>
  bool Thread<__Worker>::IsActive() const {
    return __Worker::IsActive();
  }


  template<class __Worker>
  void* Thread<__Worker>::Wrapper_(void* ptr_this) {
    __Worker* me = static_cast<__Worker*>(ptr_this);
    me->ClearStoppedStatus();
    me->Run();
    me->SetStoppedStatus();
    ::pthread_exit(0);
  }

} // namespace threads

#endif // THREADS_THREAD_HPP__
