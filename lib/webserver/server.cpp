// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "server.h"
#include "exception.h"
#include "status.h"


namespace webserver {

  unsigned int Server::max_connects_ = 1000;
  unsigned int Server::connection_timeout_ = 10;
  tbb::atomic<unsigned int> Server::servers_count_;


  Server::Server() {
    Constructor_();
  }


  Server::~Server() {
    destroy(poll_);

    if (--servers_count_ == 0) {
      if (listener_) {
        listener_->Stop();
        listener_->Join();
      }
      Status::Self()->Stop();
      Status::Self()->Join();
      Status::Destroy();
    }
  }


  Server::sptr Server::Create() {
    Server::sptr s = Server::sptr(new Server());
    return s;
  }


  void Server::SetMaxConnects(const unsigned int max_connects) {
    max_connects_ = max_connects;
  }


  void Server::SetConnectionTimeout(const unsigned int timeout) {
    connection_timeout_ = timeout;
  }


  io::Poll* Server::GetPoll() const {
    return poll_;
  }


  void Server::NewConnection(const BaseConnection::sptr& c) {
    tbb::spin_mutex::scoped_lock lock(mutex_);
    if (connections_.size() >= max_connects_) {
      base_throw0(TooManyConnectionsError);
    }

    connections_.insert(c);
  }


  void Server::DeleteConnection(const BaseConnection::wptr& c) {
    if (BaseConnection::sptr cc = c.lock()) {
      tbb::spin_mutex::scoped_lock lock(mutex_);
      connections_.erase(cc);
      activity_.erase(cc);
    }
  }


  void Server::ActivityOn(const BaseConnection::wptr& c) {
    if (BaseConnection::sptr cc = c.lock()) {
      if (std::find(activity_.begin(), activity_.end(), cc) == activity_.end()) {
        activity_.insert(cc);
      }
    }
  }


  unsigned int Server::ActiveConnections() const {
    return static_cast<unsigned int>(connections_.size());
  }


  unsigned int Server::GetConnectionTimeout() const {
    return connection_timeout_;
  }


  void Server::Perform() {
    poll_->DoPoll(1000);
    poll_->Perform();
  }


  void Server::Constructor_() {
    if (servers_count_++ == 0) {
      Status::Init(true, true);
    }

    // Open poll with additional 10 connections as a protection measure.
    if ((poll_ = io::PollKQueue::Create(max_connects_ + 10)) == 0 &&
        (poll_ = io::PollEPoll::Create(max_connects_ + 10)) == 0) {
      base_throw(IOException, "No suitable poll mechanism detected.");
    }
  }

} // namespace webserver
