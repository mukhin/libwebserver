// poll.h
// Asyncronous I/O library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#ifndef IO_POLL_H__
#define IO_POLL_H__

#include <base/prototype.h>
#include <sys/types.h>

namespace io {

  class Event;

  class Poll : public base::NonCopyable {
  public:
    virtual ~Poll() { }

    // The open max value is used when initializing libtorrent, it
    // should be less than or equal to sysconf(_SC_OPEN_MAX).
    virtual size_t OpenMax() const = 0;

    // Event::Descriptor() is guaranteed to be valid and remain constant
    // from open(...) is called to close(...) returns. The implementor
    // of this class should not open nor close the file descriptor.
    virtual void Open(Event* event) = 0;
    virtual void Close(Event* event) = 0;

    virtual int DoPoll(const int msec) = 0;
    virtual void Perform() = 0;

    // Functions for checking whetever the Event is listening to r/w/e?
    virtual bool InRead(Event* event) = 0;
    virtual bool InWrite(Event* event) = 0;
    virtual bool InError(Event* event) = 0;

    // These functions may be called on 'event's that might, or might
    // not, already be in the set.
    virtual void InsertRead(Event* event) = 0;
    virtual void InsertWrite(Event* event) = 0;
    virtual void InsertError(Event* event) = 0;

    virtual void RemoveRead(Event* event) = 0;
    virtual void RemoveWrite(Event* event) = 0;
    virtual void RemoveError(Event* event) = 0;

    // Add one for HUP? Or would that be in event?
  };

} // namespace io

#endif // IO_POLL_H__
