// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "outgoinghttpmessage.h"
#include <base/string_helpers.h>


namespace webserver {

  tbb::atomic<size_t> OutgoingHttpMessage::header_reservation_;


  OutgoingHttpMessage::OutgoingHttpMessage(const clocks::HiResTimer* timer)
  : method_(webserver::RESPONSE)
  , response_code_(webserver::HTTP_OK)
  , data_len_(0)
  , len_(0)
  , data_(0)
  , message_(0)
  , timer_(0) {
    header_.reserve(header_reservation_);
    if (timer) {
      timer_ = (new clocks::HiResTimer(*timer));
    }
  }


  OutgoingHttpMessage::OutgoingHttpMessage(const clocks::HiResTimer& timer)
  : method_(webserver::RESPONSE)
  , response_code_(webserver::HTTP_OK)
  , data_len_(0)
  , len_(0)
  , data_(0)
  , message_(0)
  , timer_(new clocks::HiResTimer(timer)) {
    header_.reserve(header_reservation_);
  }


  OutgoingHttpMessage::OutgoingHttpMessage()
  : method_(webserver::RESPONSE)
  , response_code_(webserver::HTTP_OK)
  , data_len_(0)
  , len_(0)
  , data_(0)
  , message_(0)
  , timer_(0) {
    header_.reserve(header_reservation_);
  }


  OutgoingHttpMessage::~OutgoingHttpMessage() {
    if (timer_) {
      delete timer_;
    }

    if (message_) {
      delete[] message_;
    }

    if (data_) {
      delete[] data_;
    }
  }


  void OutgoingHttpMessage::Serialize() {
    size_t pos = 0;
    if (method_ == GET) {
      len_ = uri_.length() + header_.length() + data_len_ + 19;
      message_ = new char[len_];

      ::memcpy(message_, "GET ", 4);
      pos += 4;
      ::memcpy(message_ + pos, uri_.c_str(), uri_.length());
      pos += uri_.length();
      ::memcpy(message_ + pos, "?", 1);
      pos += 1;
      ::memcpy(message_ + pos, data_, data_len_);
      pos += data_len_;
      ::memcpy(message_ + pos, " HTTP/1.1\r\n", 11);
      pos += 11;
      ::memcpy(message_ + pos, header_.c_str(), header_.length());
      pos += header_.length();
      ::memcpy(message_ + pos, "\r\n", 2);
      pos += 2;
    }
    else if (method_ == POST) {
      header_.append("Content-Length: ");
      header_.append(base::ToString(data_len_));
      header_.append("\r\n");

      len_ = uri_.length() + header_.length() + data_len_ + 19;
      message_ = new char[len_];
      ::memcpy(message_, "POST ", 5);
      pos += 5;
      ::memcpy(message_ + pos, uri_.c_str(), uri_.length());
      pos += uri_.length();
      ::memcpy(message_ + pos, " HTTP/1.1\r\n", 11);
      pos += 11;
      ::memcpy(message_ + pos, header_.c_str(), header_.length());
      pos += header_.length();
      ::memcpy(message_ + pos, "\r\n", 2);
      pos += 2;
      ::memcpy(message_ + pos, data_, data_len_);
      pos += data_len_;
    }
    // method_ == RESPONSE
    else {
      if (response_code_ == HTTP_OK && data_len_ == 0) {
        response_code_ = HTTP_NO_CONTENT;
      }
      else {
        header_.append("Content-Length: ");
        header_.append(base::ToString(data_len_));
        header_.append("\r\n");
      }

      len_ = HTTP_CODES_LENGTH[response_code_] + header_.length() + data_len_ + 14;
      message_ = new char[len_];
      ::memcpy(message_, "HTTP/1.1 ", 9);
      pos += 9;
      ::memcpy(message_ + pos, HTTP_CODES[response_code_][HTTP_MESSAGE], HTTP_CODES_LENGTH[response_code_]);
      pos += HTTP_CODES_LENGTH[response_code_];
      ::memcpy(message_ + pos, "\r\n", 2);
      pos += 2;
      ::memcpy(message_ + pos, header_.c_str(), header_.length());
      pos += header_.length();
      ::memcpy(message_ + pos, "\r\n", 2);
      pos += 2;
      ::memcpy(message_ + pos, data_, data_len_);
      pos += data_len_;
    }

    message_[pos] = 0;
    UpdateReservation_(header_.length());
  }


  const char* OutgoingHttpMessage::GetSerializedMessage() const {
    return message_;
  }


  size_t OutgoingHttpMessage::GetLength() const {
    return len_ - 1;
  }


  void OutgoingHttpMessage::SetPersistence(const bool is_persistent) {
    OutgoingMessage::SetPersistence(is_persistent);
    if (is_persistent) {
      header_.append("Connection: keep-alive\r\n");
    }
    else {
      header_.append("Connection: close\r\n");
    }
  }


  HttpCode OutgoingHttpMessage::GetResponseCode() const {
    return response_code_;
  }


  void OutgoingHttpMessage::SetMethod(const HttpMethod method) {
    method_ = method;
  }


  void OutgoingHttpMessage::SetResponseCode(const HttpCode code) {
    response_code_ = code;
  }


  void OutgoingHttpMessage::AddHeader(const char* key, const char* value) {
    header_.append(key);
    header_.append(": ");
    header_.append(value);
    header_.append("\r\n");
  }


  void OutgoingHttpMessage::SetData(const char* data, const size_t len) {
    data_ = new char[len];
    ::memcpy(data_, data, len);
    data_len_ = len;
  }


  clocks::HiResTimer* OutgoingHttpMessage::GetTimer() const {
    return timer_;
  }


  void OutgoingHttpMessage::UpdateReservation_(const size_t length) {
    if (length > header_reservation_) {
      header_reservation_ = length;
    }
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::OK(const std::string& request_id, const bool is_persistent, const clocks::HiResTimer* t) {
    sptr response = sptr(new OutgoingHttpMessage(t));
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_OK);
    response->SetPersistence(is_persistent);

    if (!request_id.empty()) {
      response->AddHeader("X-Request-Id", request_id.c_str());
    }

    response->SetData(HTTP_CODES[HTTP_OK][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_OK]);
    return response;
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::NoContent(const std::string& request_id, const bool is_persistent, const clocks::HiResTimer* t) {
    sptr response = sptr(new OutgoingHttpMessage(t));
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_NO_CONTENT);
    response->SetPersistence(is_persistent);

    if (!request_id.empty()) {
      response->AddHeader("X-Request-Id", request_id.c_str());
    }

    response->SetData(HTTP_CODES[HTTP_NO_CONTENT][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_NO_CONTENT]);
    return response;
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::BadRequest() {
    sptr response = sptr(new OutgoingHttpMessage());
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_BAD_REQUEST);
    response->SetPersistence(false);
    response->SetData(HTTP_CODES[HTTP_BAD_REQUEST][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_BAD_REQUEST]);
    return response;
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::BadRequest(const std::string& request_id) {
    sptr response = sptr(new OutgoingHttpMessage());
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_BAD_REQUEST);
    response->SetPersistence(false);

    if (!request_id.empty()) {
        response->AddHeader("X-Request-Id", request_id.c_str());
      }

    response->SetData(HTTP_CODES[HTTP_BAD_REQUEST][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_BAD_REQUEST]);
    return response;
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::Forbidden(const clocks::HiResTimer* t) {
    sptr response = sptr(new OutgoingHttpMessage(t));
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_FORBIDDEN);
    response->SetPersistence(false);
    response->SetData(HTTP_CODES[HTTP_FORBIDDEN][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_FORBIDDEN]);
    return response;
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::Forbidden(const std::string& request_id, const clocks::HiResTimer* t) {
    sptr response = sptr(new OutgoingHttpMessage(t));
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_FORBIDDEN);
    response->SetPersistence(false);

    if (!request_id.empty()) {
      response->AddHeader("X-Request-Id", request_id.c_str());
    }

    response->SetData(HTTP_CODES[HTTP_FORBIDDEN][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_FORBIDDEN]);
    return response;
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::NotFound(const bool is_persistent, const clocks::HiResTimer* t) {
    sptr response = sptr(new OutgoingHttpMessage(t));
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_NOT_FOUND);
    response->SetPersistence(is_persistent);
    response->SetData(HTTP_CODES[HTTP_NOT_FOUND][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_NOT_FOUND]);
    return response;
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::NotFound(const std::string& content, const bool is_persistent, const clocks::HiResTimer* t) {
    sptr response = sptr(new OutgoingHttpMessage(t));
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_NOT_FOUND);
    response->SetPersistence(is_persistent);

    if (!content.empty()) {
      response->SetData(content.c_str(), content.length());
    }
    else {
      response->SetData(HTTP_CODES[HTTP_NOT_FOUND][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_NOT_FOUND]);
    }

    return response;
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::NotFound(const std::string& request_id, const std::string& content, const bool is_persistent, const clocks::HiResTimer* t) {
    sptr response = sptr(new OutgoingHttpMessage(t));
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_NOT_FOUND);
    response->SetPersistence(is_persistent);

    if (!request_id.empty()) {
      response->AddHeader("X-Request-Id", request_id.c_str());
    }

    if (!content.empty()) {
      response->SetData(content.c_str(), content.length());
    }
    else {
      response->SetData(HTTP_CODES[HTTP_NOT_FOUND][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_NOT_FOUND]);
    }

    return response;
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::NotAcceptable(const bool is_persistent, const clocks::HiResTimer* t) {
    sptr response = sptr(new OutgoingHttpMessage(t));
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_NOT_ACCEPTABLE);
    response->SetPersistence(is_persistent);
    response->SetData(HTTP_CODES[HTTP_NOT_ACCEPTABLE][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_NOT_ACCEPTABLE]);
    return response;
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::NotAcceptable(const std::string& content, const bool is_persistent, const clocks::HiResTimer* t) {
    sptr response = sptr(new OutgoingHttpMessage(t));
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_NOT_ACCEPTABLE);
    response->SetPersistence(is_persistent);

    if (!content.empty()) {
      response->SetData(content.c_str(), content.length());
    }
    else {
      response->SetData(HTTP_CODES[HTTP_NOT_ACCEPTABLE][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_NOT_ACCEPTABLE]);
    }

    return response;
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::NotAcceptable(const std::string& request_id, const std::string& content, const bool is_persistent, const clocks::HiResTimer* t) {
    sptr response = sptr(new OutgoingHttpMessage(t));
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_NOT_ACCEPTABLE);
    response->SetPersistence(is_persistent);

    if (!request_id.empty()) {
      response->AddHeader("X-Request-Id", request_id.c_str());
    }

    if (!content.empty()) {
      response->SetData(content.c_str(), content.length());
    }
    else {
      response->SetData(HTTP_CODES[HTTP_NOT_ACCEPTABLE][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_NOT_ACCEPTABLE]);
    }

    return response;
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::RequestTimeout(const std::string& request_id, const bool is_persistent, const clocks::HiResTimer* t) {
    sptr response = sptr(new OutgoingHttpMessage(t));
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_TIMEOUT);
    response->SetPersistence(is_persistent);

    if (!request_id.empty()) {
      response->AddHeader("X-Request-Id", request_id.c_str());
    }

    response->SetData(HTTP_CODES[HTTP_TIMEOUT][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_TIMEOUT]);
    return response;
  }


  OutgoingHttpMessage::sptr OutgoingHttpMessage::TooManyConnections(const clocks::HiResTimer* t) {
    sptr response = sptr(new OutgoingHttpMessage(t));
    response->SetMethod(RESPONSE);
    response->SetResponseCode(HTTP_TOO_MANY_CONNECTIONS);
    response->SetPersistence(false);

    response->SetData(HTTP_CODES[HTTP_TOO_MANY_CONNECTIONS][HTTP_MESSAGE], HTTP_CODES_LENGTH[HTTP_TOO_MANY_CONNECTIONS]);
    return response;
  }

} // namespace webserver
