// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "message.h"

namespace webserver {

  IncomingMessage::IncomingMessage() { }


  IncomingMessage::~IncomingMessage() { }


  clocks::HiResTimer& IncomingMessage::GetTimer() {
    return timer_;
  }


  void IncomingMessage::SetTimer(const clocks::HiResTimer& timer) {
    timer_ = timer;
  }


  OutgoingMessage::OutgoingMessage()
  : is_persistent_(false) { }


  OutgoingMessage::~OutgoingMessage() { }


  void OutgoingMessage::SetPersistence(const bool is_persistent) {
    is_persistent_ = is_persistent;
  }


  bool OutgoingMessage::IsPersistent() const {
    return is_persistent_;
  }


  clocks::HiResTimer& OutgoingMessage::GetTimer() {
    return timer_;
  }


  void OutgoingMessage::SetTimer(const clocks::HiResTimer& timer) {
    timer_ = timer;
  }

} // namespace webserver
