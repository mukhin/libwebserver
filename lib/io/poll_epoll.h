// poll_epoll.h
// Asyncronous I/O library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#ifndef IO_POLL_EPOLL_H__
#define IO_POLL_EPOLL_H__

#include "poll.h"

#include <inttypes.h>
#include <vector>


struct epoll_event;

namespace io {

  class PollEPoll : public Poll {
  public:
    typedef std::vector<uint32_t> Table;

    static PollEPoll* Create(const int max_open_sockets);
    virtual ~PollEPoll();

    int Descriptor() const { return fd_; }

#ifdef HAVE_EPOLL
    int DoPoll(const int msec);
    void Perform();

    virtual size_t OpenMax() const;

    // Event::Descriptor() is guaranteed to be valid and remain constant
    // from open(...) is called to close(...) returns.
    virtual void Open(Event* event);
    virtual void Close(Event* event);

    // Functions for checking whetever the torrent::Event is listening to r/w/e?
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
#else // HAVE_EPOLL
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
#endif // HAVE_EPOLL

  private:
#ifdef HAVE_EPOLL
    PollEPoll(const int fd, const int max_events, const int max_open_sockets);
#else // HAVE_EPOLL
    PollEPoll(const int fd, const int max_events, const int max_open_sockets) __attribute__((noreturn));
#endif // HAVE_EPOLL

    uint32_t EventMask_(Event* e);
    void SetEventMask_(Event* e, uint32_t m);
    void Modify_(Event* event, const int op, const uint32_t mask);

    int fd_;
    int max_events_;
    int waiting_events_;

    Table table_;
    epoll_event* events_;
  };

}

#endif // IO_POLL_EPOLL_H__
