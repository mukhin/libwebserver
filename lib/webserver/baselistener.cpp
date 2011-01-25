// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "server.h"

#include <base/exception.h>
#include <io/poll.h>


namespace webserver {

  BaseListener::BaseListener()
  : poll_(0) { }


  BaseListener::~BaseListener() {
    if (Descriptor().IsValid()) {
      Close();
    }
  }


  void BaseListener::AddHandler(const Server::sptr& server) {
    tbb::spin_mutex::scoped_lock lock(mutex_);
    handlers_.push_back(server);
    current_handler_ = 0;
  }


  BaseListener::ServerSPtr& BaseListener::GetCurrentHandler() {
    return handlers_[current_handler_];
  }


  bool BaseListener::Open(const std::string& host, const uint16_t port, const unsigned int backlog) {
    sockets::SocketAddress address(host, port);

    if (!Descriptor().Open() ||
        !Descriptor().SetNonBlocking() ||
        !Descriptor().SetReuseAddress(true)) {
      base_throw(IOException, "Could not allocate socket for listening.");
    }

    if ((poll_ = io::PollKQueue::Create(Descriptor().Descriptor() + 1)) == 0 &&
        (poll_ = io::PollEPoll::Create(Descriptor().Descriptor() + 1)) == 0) {
      base_throw(IOException, "No suitable poll mechanism detected.");
    }

    if (Descriptor().Bind(address) && Descriptor().Listen(backlog)) {
      SetState(Event::STATE_LISTENING);
      poll_->Open(this);
      poll_->InsertRead(this);
      poll_->InsertError(this);
      return true;
    }

    // Cleanup on fail.
    Close();
    return false;
  }


  void BaseListener::Close() {
    if (!Descriptor().IsValid()) {
      return;
    }

    poll_->RemoveRead(this);
    poll_->RemoveError(this);
    poll_->Close(this);

    Descriptor().Close();
    delete poll_;
  }


  void BaseListener::EventRead() {
    sockets::SocketFd fd;
    while ((fd = Descriptor().Accept()).IsValid()) {
      ProcessEventRead_(fd);
      if (++current_handler_ == handlers_.size()) {
        current_handler_ = 0;
      }
    }
  }


  void BaseListener::EventWrite() {
    base_throw(IOException, "Listener does not support writes.");
  }


  void BaseListener::EventError() {
    base_throw(IOException, "Listener received an error event.");
  }


  void BaseListener::Run() {
    while (!ShouldStop()) {
      poll_->DoPoll(10000);
      poll_->Perform();
    }

    Close();
  }

} // namespace webserver
