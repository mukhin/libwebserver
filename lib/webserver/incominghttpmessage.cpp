// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "incominghttpmessage.h"
#include "exception.h"
#include <cstring/conversions.h>


namespace webserver {

  IncomingHttpMessage::IncomingHttpMessage()
  : method_(webserver::RESPONSE)
  , length_(0)
  , is_persistent_(false) { }


  IncomingHttpMessage::~IncomingHttpMessage() { }


  HttpMethod IncomingHttpMessage::GetMethod() const {
    return method_;
  }


  const std::string& IncomingHttpMessage::GetUri() const {
    return uri_;
  }


  bool IncomingHttpMessage::IsPersistent() const {
    return is_persistent_;
  }


  const IncomingHttpMessage::HttpPairList& IncomingHttpMessage::GetHeaders() const {
    return headers_;
  }


  const IncomingHttpMessage::HttpPairList& IncomingHttpMessage::GetQueries() const {
    return queries_;
  }


  bool IncomingHttpMessage::FindHeader(const char* key, IncomingHttpMessage::HttpPair& header) const {
    for (HttpPairList::const_iterator i = headers_.begin(); i != headers_.end(); ++i) {
      if (::strcasecmp(key, (**i).key.c_str()) == 0) {
        header = *i;
        return true;
      }
    }

    return false;
  }


  bool IncomingHttpMessage::FindQuery(const char* key, IncomingHttpMessage::HttpPair& query) const {
    for (HttpPairList::const_iterator i = queries_.begin(); i != queries_.end(); ++i) {
      if (::strcasecmp(key, (**i).key.c_str()) == 0) {
        query = *i;
        return true;
      }
    }

    return false;
  }


  bool IncomingHttpMessage::GetRequestId(IncomingHttpMessage::HttpPair& reqid) const {
    reqid = request_id_;
    return request_id_;
  }


  //
  // Deserialization wrapper.
  //
  // Returns:
  //   true, on success;
  //   false, on incomplete data.
  //

  bool IncomingHttpMessage::Deserialize(IncomingHttpMessage::sptr& request, const base::CString& data, size_t& eof) {
    if (data.IsEmpty()) {
      return false;
    }

    request = sptr(new IncomingHttpMessage());

    if (!request->DeserializeMethod_(data, eof)) {
      return false;
    }

    if (!request->DeserializeHeader_(data, eof)) {
      return false;
    }

    if (!request->DeserializeQuery_(data, eof)) {
      return false;
    }

    return true;
  }

  //
  // Method to deserialize first line of HTTP request.
  //
  // Returns:
  //   true, on success;
  //   false, on incomplete data.
  //
  // Throws:
  //   DeserializationError, on erroneous data.
  //

  bool IncomingHttpMessage::DeserializeMethod_(const base::CString& request, size_t& eof) {
    eof = request.Pos("\r\n");
    size_t p = 0;

    if (eof == base::CString::npos) {
      return false;
    }
    else {
      eof += 2;
    }

    // Parse method: GET, POST or response.
    {
      // Methods are ordered by usage frequency from most used to least used: POST, GET, HTTP/1.1, HTTP/1.0.
      if (request.Compare("POST ", 0, 5) == 0) {
        method_ = POST;
        p += 5;
      }
      else if (request.Compare("GET ", 0, 4) == 0) {
        method_ = GET;
        p += 4;
      }
      else if (request.Compare("HTTP/1.1 ", 0, 9) == 0) {
        method_ = RESPONSE;
        p += 9;
      }
      else if (request.Compare("HTTP/1.0 ", 0, 9) == 0) {
        method_ = RESPONSE;
        p += 9;
      }
      else {
        base::CString error("Invalid method: ", 16);
        error.Append(request.Substring(0, eof));
        base_throw(DeserializationError, error.Str());
      }
    }

    // Check if response code is 200.
    if (method_ == RESPONSE) {
      if (request.Compare("200", p, 3) != 0) {
        base::CString error("Response code is not 200: ", 26);
        error.Append(request.Substring(0, eof));
        base_throw(DeserializationError, error.Str());
      }
    }
    else { // POST or GET
      size_t pp = request.Pos(" HTTP/1.", p);
      if (pp == base::CString::npos) {
        base::CString error("Invalid protocol: ", 18);
        error.Append(request.Substring(0, eof));
        base_throw(DeserializationError, error.Str());
      }

      uri_ = request.Substring(p, pp - p).Str();
    }

    return true;
  }

  //
  // Method to deserialize rest of the HTTP header after the first line.
  //
  // Returns:
  //   true, on success;
  //   false, on incomplete data.
  //
  // Throws:
  //   DeserializationError, on erroneous data.
  //

  bool IncomingHttpMessage::DeserializeHeader_(const base::CString& request, size_t& eof) {
    if (eof >= request.Length()) {
      return false;
    }

    const char* p = request.Str() + eof;
    bool header_line_end = false;
    bool header_end = false;

    while (*p) {
      if (eof + 2 > request.Length()) {
        return false;
      }

      if (*p == '\r' && *(p + 1) == '\n' && header_line_end) {
        eof += 2;
        header_end = true;
        break;
      }
      else {
        header_line_end = false;
      }

      HttpPair pair = HttpPair(new _HttpPair());

      // Get key.
      while (*p && *p != '\r') {
        if (*p == ':' && *(p + 1) == ' ') {
          p += 2;
          eof += 2;
          break;
        }

        pair->key.append(1, *p++);
        ++eof;
      }

      // Get value if key is present.
      if (!pair->key.empty()) {
        while (*p) {
          if (*p == '\r' && *(p + 1) == '\n') {
            p += 2;
            eof += 2;
            header_line_end = true;
            break;
          }

          pair->value.append(1, *p++);
          ++eof;
        }

        if (!pair->value.empty()) {
          headers_.push_back(pair);

          if (::strcasecmp(pair->key.c_str(), "content-length") == 0) {
            length_ = ::strtoul(pair->value.c_str(), 0, 10);
          }
          else if (::strcasecmp(pair->key.c_str(), "connection") == 0) {
            is_persistent_ = ::strcasecmp(pair->value.c_str(), "keep-alive") == 0;
          }
          else if (::strcasecmp(pair->key.c_str(), "x-request-id") == 0) {
            request_id_ = pair;
          }
        }
      }
    }

    return header_end;
  }

  //
  // Method to deserialize query part of URI in GET request, or content in POST request.
  //
  // Returns:
  //   true, on success;
  //   false, on incomplete data.
  //
  // Throws:
  //   DeserializationError, on erroneous data.
  //

  bool IncomingHttpMessage::DeserializeQuery_(const base::CString& request, size_t& eof) {
    const char* p;

    if (method_ == GET) {
      p = ::strstr(uri_.c_str(), "?");
      eof += length_;

      if (p == 0) {
        return true;
      }
    }
    else if (method_ == POST || method_ == RESPONSE) {
      if (eof + length_ > request.Length()) {
        return false;
      }

      p = request.Str() + eof;
      eof += length_;
    }
    else {
      eof += length_;
      return true;
    }

    while (*p) {
      HttpPair pair = HttpPair(new _HttpPair());

      while (*p == '?' || *p == '&') {
        ++p;
      }

      // Get key if key is present.
      while (*p != '=' && *p) {
        pair->key.append(1, *p++);
      }

      while (*p == '=') {
        p++;
      }

      // Get value.
      if (!pair->key.empty()) {
        while (*p != '&' && *p) {
          pair->value.append(1, *p++);
        }

        if (!pair->value.empty()) {
          queries_.push_back(pair);
        }
      }
    }

    return true;
  }

} // namespace webserver
