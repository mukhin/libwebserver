// poll_epoll.cpp
// Asyncronous I/O library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#include "poll_epoll.h"
#include "event.h"

#ifdef HAVE_SYS_EPOLL_H
extern "C" {
#include <sys/epoll.h>
#include <unistd.h>
}
#endif

#include <base/c_format.h>
#include <base/exception.h>
#include <base/string_helpers.h>


using std::string;

namespace io {

#ifdef HAVE_EPOLL

  PollEPoll::PollEPoll(const int fd, const int max_events, const int max_open_sockets)
  : fd_(fd)
  , max_events_(max_events)
  , waiting_events_(0)
  , events_(new epoll_event[max_events_]) {
    table_.resize(max_open_sockets);
  }

  PollEPoll::~PollEPoll() {
    table_.clear();
    delete [] events_;
    ::close(fd_);
  }

  PollEPoll* PollEPoll::Create(const int max_open_sockets) {
    int fd = ::epoll_create(max_open_sockets);

    if (fd == -1) {
      return 0;
    }

    return new PollEPoll(fd, 1024, max_open_sockets);
  }

  int PollEPoll::DoPoll(const int msec) {
    int nfds = ::epoll_wait(fd_, events_, max_events_, msec);

    if (nfds == -1) {
      return -1;
    }

    return waiting_events_ = nfds;
  }

  // We check table_ to make sure the Event is still listening to the
  // event, so it is safe to remove Event's while in working.
  //
  // TODO: Do we want to guarantee if the Event has been removed from
  // some event but not closed, it won't call that event? Think so...
  void PollEPoll::Perform() {
    for (epoll_event *itr = events_, *last = events_ + waiting_events_; itr != last; ++itr) {
      Event* event = static_cast<Event*>(itr->data.ptr);

      // Each branch must check for data.ptr != 0 to allow the socket
      // to remove itself between the calls.
      //
      // TODO: Make it so that it checks that read/write is wanted, that
      // it wasn't removed from one of them but not closed.

      if (itr->events & EPOLLERR && event && !event->IsInactive() && EventMask_(event) & EPOLLERR) {
        event->EventError();
      }

      if (itr->events & EPOLLIN && event && !event->IsInactive() && EventMask_(event) & EPOLLIN) {
        event->EventRead();
      }

      if (itr->events & EPOLLOUT && event && !event->IsInactive() && EventMask_(event) & EPOLLOUT) {
        event->EventWrite();
      }
    }

    waiting_events_ = 0;
  }

  size_t PollEPoll::OpenMax() const {
    return table_.size();
  }

  void PollEPoll::Open(Event* event) {
    if (EventMask_(event) != 0) {
      std::string ev;
      if (InRead(event)) {
        ev += "r";
      }

      if (InWrite(event)) {
        ev += "w";
      }

      if (InError(event)) {
        ev += "e";
      }

      base_throw(IOException, c_format("PollEPoll::Open(...) called but the file descriptor %d:%s is active ",
                                       event->Descriptor(), ev.c_str()));
    }
  }

  void PollEPoll::Close(Event* event) {
    if (EventMask_(event) != 0) {
      string ev;
      if (InRead(event)) {
        ev += "r";
      }

      if (InWrite(event)) {
        ev += "w";
      }

      if (InError(event)) {
        ev += "e";
      }

      base_throw(IOException, c_format("PollEPoll::Close(...) called but the file descriptor %d:%s is active",
                                       event->Descriptor(), ev.c_str()));
    }

    for (epoll_event *itr = events_, *last = events_ + waiting_events_; itr != last; ++itr) {
      if (itr->data.ptr == event) {
        itr->data.ptr = 0;
      }
    }
  }

  // Use custom defines for EPOLL* to make the below code compile with and with epoll.
  bool PollEPoll::InRead(Event* event) {
    return EventMask_(event) & EPOLLIN;
  }

  bool PollEPoll::InWrite(Event* event) {
    return EventMask_(event) & EPOLLOUT;
  }

  bool PollEPoll::InError(Event* event) {
    return EventMask_(event) & EPOLLERR;
  }

  void PollEPoll::InsertRead(Event* event) {
    Modify_(event,
            EventMask_(event) ? EPOLL_CTL_MOD : EPOLL_CTL_ADD,
            EventMask_(event) | EPOLLIN);
  }

  void PollEPoll::InsertWrite(Event* event) {
    Modify_(event,
            EventMask_(event) ? EPOLL_CTL_MOD : EPOLL_CTL_ADD,
            EventMask_(event) | EPOLLOUT);
  }

  void PollEPoll::InsertError(Event* event) {
    Modify_(event,
            EventMask_(event) ? EPOLL_CTL_MOD : EPOLL_CTL_ADD,
            EventMask_(event) | EPOLLERR);
  }

  void PollEPoll::RemoveRead(Event* event) {
    uint32_t mask = EventMask_(event) & ~EPOLLIN;
    Modify_(event, mask ? EPOLL_CTL_MOD : EPOLL_CTL_DEL, mask);
  }

  void PollEPoll::RemoveWrite(Event* event) {
    uint32_t mask = EventMask_(event) & ~EPOLLOUT;
    Modify_(event, mask ? EPOLL_CTL_MOD : EPOLL_CTL_DEL, mask);
  }

  void
  PollEPoll::RemoveError(Event* event) {
    uint32_t mask = EventMask_(event) & ~EPOLLERR;
    Modify_(event, mask ? EPOLL_CTL_MOD : EPOLL_CTL_DEL, mask);
  }

  uint32_t PollEPoll::EventMask_(Event* e) {
    uint64_t fd = e->Descriptor();
    if (fd >= table_.size()) {
      return 0;
    }
    return table_[fd];
  }

  void PollEPoll::SetEventMask_(Event* e, uint32_t m) {
    table_[e->Descriptor()] = m;
  }

  void PollEPoll::Modify_(Event* event, int op, uint32_t mask) {
    if (EventMask_(event) == mask) {
      return;
    }

    epoll_event e;
    e.data.u64 = 0; // Make valgrind happy? Remove please.
    e.data.ptr = event;
    e.events = mask;

    SetEventMask_(event, mask);

    if (::epoll_ctl(fd_, op, event->Descriptor(), &e)) {
      base_throw(InternalError, "epoll_ctl call failed");
    }
  }

#else // HAVE_EPOLL

  PollEPoll::~PollEPoll() { }

  PollEPoll* PollEPoll::Create(const int /* max_open_sockets */) {
    return 0;
  }

  void PollEPoll::Open(Event* /* event */) {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

  void PollEPoll::Close(Event* /* event */) {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

  bool PollEPoll::InRead(Event* /* event */) {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

  bool PollEPoll::InWrite(Event* /* event */) {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

  bool PollEPoll::InError(Event* /* event */) {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

  void PollEPoll::InsertRead(Event* /* event */) {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

  void PollEPoll::InsertWrite(Event* /* event */) {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

  void PollEPoll::InsertError(Event* /* event */) {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

  void PollEPoll::RemoveRead(Event* /* event */) {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

  void PollEPoll::RemoveWrite(Event* /* event */) {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

  void PollEPoll::RemoveError(Event* /* event */) {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }


  int PollEPoll::DoPoll(const int /* msec */) {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

  void PollEPoll::Perform() {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

  size_t PollEPoll::OpenMax() const {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

  PollEPoll::PollEPoll(const int /* fd */, const int /* max_events */, const int /* max_open_sockets */) {
    base_throw(InternalError, "An PollEPoll function was called, but it is disabled.");
  }

#endif // USE_EPOLL

} // namespace io
