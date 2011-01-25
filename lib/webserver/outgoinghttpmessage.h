// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef WEBSERVER_OUTGOING_HTTP_MESSAGE_H__
#define WEBSERVER_OUTGOING_HTTP_MESSAGE_H__

#include "httptypes.h"
#include "message.h"
#include <tbb/atomic.h>


namespace webserver {

  class OutgoingHttpMessage : public OutgoingMessage {
  public:
    typedef std::tr1::shared_ptr<OutgoingHttpMessage> sptr;

    OutgoingHttpMessage(const clocks::HiResTimer* timer);
    OutgoingHttpMessage(const clocks::HiResTimer& timer);
    OutgoingHttpMessage();
    ~OutgoingHttpMessage();

    void Serialize();
    const char* GetSerializedMessage() const;
    size_t GetLength() const;

    void SetPersistence(const bool is_persistent);
    HttpCode GetResponseCode() const;
    void SetResponseCode(const HttpCode code);
    void SetMethod(const HttpMethod method);
    void AddHeader(const char* key, const char* value);
    void SetData(const char* data, const size_t len);
    clocks::HiResTimer* GetTimer() const;
    bool ConnectionShouldBeClosed() const;

    static sptr OK(const std::string& request_id, const bool is_persistent, const clocks::HiResTimer* t = 0);
    static sptr NoContent(const std::string& request_id, const bool is_persistent, const clocks::HiResTimer* t = 0);
    static sptr BadRequest();
    static sptr BadRequest(const std::string& request_id);
    static sptr Forbidden(const clocks::HiResTimer* t = 0);
    static sptr Forbidden(const std::string& request_id, const clocks::HiResTimer* t = 0);
    static sptr NotFound(const bool is_persistent, const clocks::HiResTimer* t = 0);
    static sptr NotFound(const std::string& content, const bool is_persistent, const clocks::HiResTimer* t = 0);
    static sptr NotFound(const std::string& request_id, const std::string& content, const bool is_persistent, const clocks::HiResTimer* t = 0);
    static sptr NotAcceptable(const bool is_persistent, const clocks::HiResTimer* t = 0);
    static sptr NotAcceptable(const std::string& content, const bool is_persistent, const clocks::HiResTimer* t = 0);
    static sptr NotAcceptable(const std::string& request_id, const std::string& content, const bool is_persistent, const clocks::HiResTimer* t = 0);
    static sptr RequestTimeout(const std::string& request_id, const bool is_persistent, const clocks::HiResTimer* t = 0);
    static sptr TooManyConnections(const clocks::HiResTimer* t = 0);

  private:
    static void UpdateReservation_(const size_t length);

    HttpMethod method_;
    HttpCode response_code_;

    std::string uri_;
    std::string header_;
    size_t data_len_;
    size_t len_;
    char* data_;
    char* message_;

    clocks::HiResTimer* timer_;

    static tbb::atomic<size_t> header_reservation_;
  };

} // namespace webserver

#endif // WEBSERVER_OUTGOING_HTTP_MESSAGE_H__
