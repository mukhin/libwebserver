// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "baseconnection.h"
#include "exception.h"
#include "server.h"
#include "status.h"
#include <sockets/exception.h>

namespace webserver {

  BaseConnection::BaseConnection(const std::string& local, sockets::SocketAddress& remote, ServerSPtr& handler)
  : is_persistent_(false)
  , buffer_length_(1500)
  , buffer_(buffer_length_)
  , handler_(handler)
  , logger_(log4cpp::Category::getInstance("webserver")) {
    sockets::SocketFd fd;
    if (!fd.Open() || !fd.SetReuseAddress(true)) {
      base_throw(IOException, "Could not allocate socket for communication");
    }

    if (!local.empty() && !fd.Bind(local)) {
      base_throw(IOException, "Could not bind socket to address");
    }

    SetDescriptor(fd);
    SetOptions_();

    if (!fd.Connect(remote)) {
      if (errno != EINPROGRESS) {
        fd.Close();
        base_throw(IOException, "Could not connect to remote peer");
      }

      fd_set rset, wset;
      timeval tv;
      FD_ZERO(&rset);
      FD_ZERO(&wset);
      FD_SET(Descriptor().Descriptor(), &rset);
      FD_SET(Descriptor().Descriptor(), &wset);
      tv.tv_sec = handler_->GetConnectionTimeout();
      tv.tv_usec = 0;

      if (::select(fd.Descriptor() + 1, &rset, &wset, 0, &tv) == 0) {
        Descriptor().Close();
        base_throw(IOException, "Connection timed out");
      }
    }
  }


  BaseConnection::BaseConnection(const sockets::SocketFd& fd, ServerSPtr& handler)
  : is_persistent_(false)
  , buffer_length_(1500)
  , buffer_(buffer_length_)
  , handler_(handler)
  , logger_(log4cpp::Category::getInstance("webserver")) {
    SetDescriptor(fd);
    SetOptions_();
  }


  BaseConnection::~BaseConnection() {
    Close();
  }


  void BaseConnection::EventRead() {
    if (state_ != STATE_CONNECTED) {
      return;
    }

    // Allocate buffer for socket to write to.
    if (buffer_.Reserved() < buffer_length_ + buffer_.Length()) {
      buffer_.Reserve(buffer_length_ - buffer_.Reserved());
    }

    ssize_t len;
    try {
      // Append socket's data at buffer's end.
      len = ReadStream(buffer_.End(), buffer_length_);
      buffer_.AdjustLength(len);
    }
    catch (const sockets::SocketException& e) {
      GetLogger_().warn("ReadStream failed with error: " + e.why());
      Close();
      return;
    }

    if (len == 0) {
      buffer_.Clear();
      return;
    }

    ProcessEventRead_(buffer_);
  }


  void BaseConnection::EventWrite() {
    if (state_ != STATE_CONNECTED) {
      return;
    }

    if (outgoing_.empty()) {
      handler_->GetPoll()->RemoveWrite(this);
      return;
    }

    std::list<OutgoingMessage::sptr> outgoing;
    outgoing.swap(outgoing_);

    for (std::list<OutgoingMessage::sptr>::const_iterator i = outgoing.begin(); i != outgoing.end(); ++i) {
      if (state_ != STATE_CONNECTED) {
        return;
      }

      const OutgoingMessage::sptr& message = *i;
      const char* response = message->GetSerializedMessage();

      try {
        while (WriteStream(response, message->GetLength()) == 0) {
          threads::Yield();
        }
      }
      catch (const sockets::SocketException& e) {
        GetLogger_().warnStream() << "WriteStream failed with error: " << e.why();
        Close();
        break;
      }
      catch (const std::exception& e) {
        GetLogger_().warnStream() << "WriteStream failed with error: " << e.what();
        Close();
        break;
      }

      AfterEventWrite_(message);

      if (!message->IsPersistent()) {
        Close();
        break;
      }
    }
  }


  void BaseConnection::EventError() {
    if (GetState() == STATE_INACTIVE) {
      base_throw(IOException, "EventError() called on an inactive worker connection.");
    }

    // Handle network errors.
    logger_.warn("Poll reports network error.");
    Close();
  }


  bool BaseConnection::GetMessages(std::list<IncomingMessage::sptr>& messages) {
    if (state_ != STATE_CONNECTED || incoming_.empty()) {
      return false;
    }

    messages.swap(incoming_);
    return true;
  }


  void BaseConnection::SendMessage(const OutgoingMessage::sptr& message) {
    if (state_ != STATE_CONNECTED) {
      return;
    }

    message->Serialize();
    outgoing_.push_back(message);
    handler_->GetPoll()->InsertWrite(this);
  }


  void BaseConnection::SetPersistence(const bool is_persistent) {
    is_persistent_ = is_persistent;
  }


  bool BaseConnection::IsPersistent() const {
    return is_persistent_;
  }


  void BaseConnection::Initialize() {
    if (!handler_) {
      Descriptor().Close();
      return;
    }

    if (handler_->GetPoll()) {
      handler_->GetPoll()->Open(this);
      handler_->GetPoll()->InsertRead(this);
      handler_->GetPoll()->InsertWrite(this);
      handler_->GetPoll()->InsertError(this);
    }
    else {
      Descriptor().Close();
      return;
    }

    SetState(STATE_CONNECTED);
  }


  void BaseConnection::Close() {
    if (state_ == STATE_CLOSING) {
      return;
    }

    SetState(STATE_CLOSING);
    BaseConnection::sptr c = weak_this_.lock();
    handler_->DeleteConnection(weak_this_);

    if (handler_->GetPoll()) {
      handler_->GetPoll()->RemoveRead(this);
      handler_->GetPoll()->RemoveWrite(this);
      handler_->GetPoll()->RemoveError(this);
      handler_->GetPoll()->Close(this);
    }

    if (Descriptor().IsValid()) {
      Descriptor().Close();
    }
  }


  void BaseConnection::SetWeakThis_(const wptr& weak_this) {
    weak_this_ = weak_this;
  }


  void BaseConnection::PushIncoming_(const IncomingMessage::sptr& incoming) {
    incoming_.push_back(incoming);
  }


  void BaseConnection::MarkActivity_() {
    handler_->ActivityOn(weak_this_);
  }


  log4cpp::Category& BaseConnection::GetLogger_() {
    return logger_;
  }


  void BaseConnection::SetOptions_() {
    if (!Descriptor().SetNoSignal(true)) {
      logger_.warn("Could not disable SIGPIPE signaling on socket errors.");
      Descriptor().Close();
      return;
    }

    if (!Descriptor().SetNoDelay(true)) {
      logger_.warn("Could not disable Nagle algorithm on socket.");
      Descriptor().Close();
      return;
    }

    if (!Descriptor().SetNonBlocking()) {
      logger_.warn("Could not set non-blocking socket operation.");
      Descriptor().Close();
      return;
    }

    if (!Descriptor().SetSendTimeout(handler_->GetConnectionTimeout())) {
      logger_.warn("Could not set socket send timeout.");
      Descriptor().Close();
      return;
    }

    if (!Descriptor().SetReceiveTimeout(handler_->GetConnectionTimeout())) {
      logger_.warn("Could not set socket receive timeout.");
      Descriptor().Close();
      return;
    }
  }


  void BaseConnection::AfterEventWrite_(const OutgoingMessage::sptr& message) {
    static_cast<void>(message);
  }


} // namespace webserver
