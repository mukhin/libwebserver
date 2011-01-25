// poll_kqueue.h
// Asyncronous I/O library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#ifndef IO_POLL_KQUEUE_H__
#define IO_POLL_KQUEUE_H__

#include "poll.h"

#include <inttypes.h>
#include <vector>


struct kevent;

namespace io {

  class PollKQueue : public Poll {
  public:
    typedef std::vector<uint32_t> Table;

    static const uint32_t flag_read = (1 << 0);
    static const uint32_t flag_write = (1 << 1);
    static const uint32_t flag_error = (1 << 2);

    virtual ~PollKQueue();

    static PollKQueue* Create(const int max_open_sockets);
    int Descriptor() const { return fd_; }

#ifdef HAVE_WORKING_KQUEUE
    int DoPoll(const int msec);
    void Perform();

    virtual size_t OpenMax() const;

    // Event::Descriptor() is guaranteed to be valid and remain constant
    // from open(...) is called to close(...) returns.
    virtual void Open(Event* event);
    virtual void Close(Event* event);

    // Functions for checking whetever the Event is listening to r/w/e?
    virtual bool InRead(Event* event);
    virtual bool InWrite(Event* event);
    virtual bool InError(Event* event);

    // These functions may be called on 'event's that might, or might
    // not, already be in the set.
    virtual void InsertRead(Event* event);
    virtual void InsertWrite(Event* event);
    virtual void InsertError(Event* event);

    virtual void RemoveRead(Event* event);
    virtual void RemoveWrite(Event* event);
    virtual void RemoveError(Event* event);
#else // ! HAVE_WORKING_KQUEUE
    int DoPoll(const int msec) __attribute__((noreturn));
    void Perform() __attribute__((noreturn));
    virtual size_t OpenMax() const __attribute__((noreturn));
    virtual void Open(Event* event) __attribute__((noreturn));
    virtual void Close(Event* event) __attribute__((noreturn));
    virtual bool InRead(Event* event) __attribute__((noreturn));
    virtual bool InWrite(Event* event) __attribute__((noreturn));
    virtual bool InError(Event* event) __attribute__((noreturn));
    virtual void InsertRead(Event* event) __attribute__((noreturn));
    virtual void InsertWrite(Event* event) __attribute__((noreturn));
    virtual void InsertError(Event* event) __attribute__((noreturn));
    virtual void RemoveRead(Event* event) __attribute__((noreturn));
    virtual void RemoveWrite(Event* event) __attribute__((noreturn));
    virtual void RemoveError(Event* event) __attribute__((noreturn));
#endif

  private:
#ifdef HAVE_WORKING_KQUEUE
    PollKQueue(const int fd, const int max_events, const int max_open_sockets);
#else // ! HAVE_WORKING_KQUEUE
    PollKQueue(const int fd, const int max_events, const int max_open_sockets) __attribute__((noreturn));
#endif

    uint32_t EventMask_(Event* e);
    void SetEventMask_(Event* e, uint32_t m);
    void Modify_(Event* event, const unsigned short op, const short mask);

    int fd_;
    int max_events_;
    int waiting_events_;

    Table table_;
    struct kevent* events_;
  };

} // namespace io

#endif // IO_POLL_KQUEUE_H__
