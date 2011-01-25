// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef WEBSERVER_MESSAGE_H__
#define WEBSERVER_MESSAGE_H__

#include <base/prototype.h>
#include <clock/hirestimer.h>
#include <tr1/memory>


namespace webserver {

  class IncomingMessage : public base::NonCopyable {
  public:
    typedef std::tr1::shared_ptr<IncomingMessage> sptr;

    IncomingMessage();
    virtual ~IncomingMessage();

    void SetTimer(const clocks::HiResTimer& timer);
    clocks::HiResTimer& GetTimer();

  private:
    clocks::HiResTimer timer_;
  };


  class OutgoingMessage : public base::NonCopyable {
  public:
    typedef std::tr1::shared_ptr<OutgoingMessage> sptr;

    OutgoingMessage();
    virtual ~OutgoingMessage();

    virtual void SetPersistence(const bool is_persistent);
    bool IsPersistent() const;

    void SetTimer(const clocks::HiResTimer& timer);
    clocks::HiResTimer& GetTimer();

    virtual void Serialize() = 0;
    virtual const char* GetSerializedMessage() const = 0;
    virtual size_t GetLength() const = 0;

  private:
    bool is_persistent_;
    clocks::HiResTimer timer_;
  };

} // namespace webserver

#endif // WEBSERVER_MESSAGE_H__
