// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef WEBSERVER_INCOMING_HTTP_MESSAGE_H__
#define WEBSERVER_INCOMING_HTTP_MESSAGE_H__

#include "httptypes.h"
#include "message.h"

#include <base/basicmacros.h>
#include <cstring/cstring.h>
#include <list>
#include <string>
#include <tr1/memory>


namespace webserver {

  class IncomingHttpMessage : public IncomingMessage {
  public:
    typedef struct {
      std::string key;
      std::string value;
    } _HttpPair;

    typedef std::tr1::shared_ptr<IncomingHttpMessage> sptr;
    typedef std::tr1::shared_ptr<_HttpPair> HttpPair;
    typedef std::list<HttpPair> HttpPairList;

    IncomingHttpMessage();
    ~IncomingHttpMessage();

    HttpMethod GetMethod() const;
    const std::string& GetUri() const;
    bool IsPersistent() const;

    const HttpPairList& GetHeaders() const;
    const HttpPairList& GetQueries() const;
    bool FindHeader(const char* key, HttpPair& header) const;
    bool FindQuery(const char* key, HttpPair& query) const;
    bool GetRequestId(HttpPair& reqid) const;

    static bool Deserialize(sptr& request, const base::CString& data, size_t& end);

  private:
    bool DeserializeMethod_(const base::CString& request, size_t& end);
    bool DeserializeHeader_(const base::CString& request, size_t& end);
    bool DeserializeQuery_(const base::CString& request, size_t& end);

    HttpMethod method_;
    std::string uri_;
    size_t length_;
    bool is_persistent_;

    HttpPairList headers_;
    HttpPairList queries_;
    HttpPair request_id_;
  };

}

#endif // WEBSERVER_INCOMING_HTTP_MESSAGE_H__
