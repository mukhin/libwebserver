// poll_kqueue.cpp
// Asyncronous I/O library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#include "poll_kqueue.h"
#include "event.h"
#include "functional.h"

#include <algorithm>
#include <base/c_format.h>
#include <base/string_helpers.h>
#include <cerrno>
#include <base/exception.h>

#ifdef HAVE_WORKING_KQUEUE
extern "C" {
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
}
#endif


namespace io {

#ifdef HAVE_WORKING_KQUEUE

  PollKQueue::PollKQueue(const int fd, const int max_events, const int max_open_sockets)
  : fd_(fd)
  , max_events_(max_events)
  , waiting_events_(0) {
    events_ = new struct kevent[max_events_];
    table_.resize(max_open_sockets);
  }

  PollKQueue::~PollKQueue() {
    table_.clear();
    delete [] events_;
    ::close(fd_);
  }

  PollKQueue* PollKQueue::Create(const int max_open_sockets) {
    int fd = ::kqueue();

    if (fd == -1) {
      return 0;
    }

    return new PollKQueue(fd, 1024, max_open_sockets);
  }

  int PollKQueue::DoPoll(const int msec) {
    timespec timeout = { msec / 1000, (msec % 1000) * 1000000 };

    int nfds = ::kevent(fd_, 0, 0, events_, max_events_, &timeout);

    if (nfds == -1) {
      return -1;
    }

    waiting_events_ = nfds;

    return nfds;
  }

  void PollKQueue::Perform() {
    for (struct kevent *itr = events_, *last = events_ + waiting_events_; itr != last; ++itr) {
      Event* event = static_cast<Event*>(itr->udata);

      // Error messages that can happen, when a delete fails.
      //   EBADF happens when the file discriptor has been closed,
      //   ENOENT when the file discriptor was closed and then reopened.
      //   EINVAL for some reasons not understood; EINVAL should not be returned ever, but sometimes does.
      // An error is also indicated when a callback deletes an event we are still processing.
      // In that case the data field is set to ENOENT.

      if (itr->flags == EV_ERROR) {
        int err = itr->data;
        if (err == EBADF || err == EINVAL || err == ENOENT) {
          continue;
        }

        base_throw(IOException, "kqueue error for descriptor " + base::ToString(event->Descriptor()) + " : "
                   + base::ToString(err) + " - " + ::strerror(err));
      }

      // Also check current mask.
      if (itr->filter == EVFILT_READ && event && !event->IsInactive() && EventMask_(event) & flag_read) {
        event->EventRead();
      }

      if (itr->filter == EVFILT_WRITE && event && !event->IsInactive() && EventMask_(event) & flag_write) {
        event->EventWrite();
      }
    }

    waiting_events_ = 0;
  }

  size_t PollKQueue::OpenMax() const {
    return table_.size();
  }

  void PollKQueue::Open(Event* event) {
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

      base_throw(IOException, c_format("PollKQueue::Open(...) called but the file descriptor %d:%s is active",
                 event->Descriptor(), ev.c_str()));
    }
  }

  void PollKQueue::Close(Event* event) {
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

      base_throw(IOException, c_format("PollKQueue::Close(...) called but the file descriptor %d:%s is active",
                 event->Descriptor(), ev.c_str()));
    }

    for (struct kevent *itr = events_, *last = events_ + waiting_events_; itr != last; ++itr) {
      if (itr->udata == event) {
        itr->udata = 0;
      }
    }
  }

  // Use custom defines for EPOLL* to make the below code compile with and with epoll.
  bool PollKQueue::InRead(Event* event) {
    return EventMask_(event) & flag_read;
  }

  bool PollKQueue::InWrite(Event* event) {
    return EventMask_(event) & flag_write;
  }

  bool PollKQueue::InError(Event* event) {
    return EventMask_(event) & flag_error;
  }

  void PollKQueue::InsertRead(Event* event) {
    if (EventMask_(event) & flag_read) {
      return;
    }

    SetEventMask_(event, EventMask_(event) | flag_read);

    Modify_(event, EV_ADD, EVFILT_READ);
  }

  void PollKQueue::InsertWrite(Event* event) {
    if (EventMask_(event) & flag_write) {
      return;
    }

    SetEventMask_(event, EventMask_(event) | flag_write);

    Modify_(event, EV_ADD, EVFILT_WRITE);
  }

  void PollKQueue::InsertError(Event* /* event */) { }

  void PollKQueue::RemoveRead(Event* event) {
    if (!(EventMask_(event) & flag_read)) {
      return;
    }

    SetEventMask_(event, EventMask_(event) & ~flag_read);
    Modify_(event, EV_DELETE, EVFILT_READ);
  }

  void PollKQueue::RemoveWrite(Event* event) {
    if (!(EventMask_(event) & flag_write)) {
      return;
    }

    SetEventMask_(event, EventMask_(event) & ~flag_write);
    Modify_(event, EV_DELETE, EVFILT_WRITE);
  }

  void PollKQueue::RemoveError(Event* /* event */) { }

  uint32_t PollKQueue::EventMask_(Event* e) {
    uint64_t fd = e->Descriptor();
    if (fd >= table_.size()) {
      return 0;
    }
    return table_[fd];
  }

  void PollKQueue::SetEventMask_(Event* e, uint32_t m) {
    table_[e->Descriptor()] = m;
  }

  void PollKQueue::Modify_(Event* event, const unsigned short op, const short mask) {
    struct kevent ev;
    EV_SET(&ev, event->Descriptor(), mask, op, 0, 0, event);
    if (::kevent(fd_, &ev, 1, 0, 0, 0) == -1) {
      base_throw(IOException, "PollKQueue::Modify_() error: " + errno);
    }
  }

#else // HAVE_WORKING_KQUEUE

  PollKQueue::~PollKQueue() { }

  PollKQueue* PollKQueue::Create(const int/* max_open_sockets*/) {
    return 0;
  }

  int PollKQueue::DoPoll(const int/* msec*/) {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  void PollKQueue::Perform() {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  size_t PollKQueue::OpenMax() const {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  void PollKQueue::Open(Event*/* event*/) {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  void PollKQueue::Close(Event*/* event*/) {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  bool PollKQueue::InRead(Event*/* event*/) {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  bool PollKQueue::InWrite(Event*/* event*/) {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  bool PollKQueue::InError(Event*/* event*/) {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  void PollKQueue::InsertRead(Event*/* event*/) {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  void PollKQueue::InsertWrite(Event*/* event*/) {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  void PollKQueue::InsertError(Event*/* event*/) {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  void PollKQueue::RemoveRead(Event*/* event*/) {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  void PollKQueue::RemoveWrite(Event*/* event*/) {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  void PollKQueue::RemoveError(Event*/* event*/) {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

  PollKQueue::PollKQueue(const int/* fd*/, const int/* max_events*/, const int/* max_open_sockets*/) {
    base_throw(InternalError, "An PollKQueue function was called, but it is disabled.");
  }

#endif // USE_KQUEUE

}
