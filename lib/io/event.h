// io.h
// Asyncronous I/O library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#ifndef IO_EVENT_H__
#define IO_EVENT_H__

#include <base/prototype.h>

namespace io {

  // @interface

  class Event : public base::NonCopyable {
  public:
    typedef enum {
      STATE_INACTIVE,
      STATE_LISTENING,
      STATE_CONNECTING,
      STATE_CONNECTED,
      STATE_CLOSING
    } State;

    Event() : fd_(-1), state_(STATE_INACTIVE) { }
    virtual ~Event() { }

    int Descriptor() const;
    State GetState() const;
    void SetState(const State state);
    bool IsInactive() const;
    bool IsConnected() const;

    virtual void EventRead() = 0;
    virtual void EventWrite() = 0;
    virtual void EventError() = 0;

  protected:
    int fd_;
    State state_;
  };



  // @implementation

  inline int Event::Descriptor() const {
    return fd_;
  }

  inline Event::State Event::GetState() const {
    return state_;
  }

  inline void Event::SetState(const State state) {
    state_ = state;
  }

  inline bool Event::IsInactive() const {
    return state_ == STATE_INACTIVE || state_ == STATE_CLOSING;
  }

  inline bool Event::IsConnected() const {
    return state_ == STATE_CONNECTED;
  }

} // namespace io

#endif // IO_EVENT_H__
