// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef WEBSERVER_HTTPTYPES_H__
#define WEBSERVER_HTTPTYPES_H__

#include <base/common.h>
#include <base/hash.h>
#include <base/prototype.h>
#include <cstring/cstring.h>
#include <map>
#include <vector>
#include <tr1/memory>


namespace webserver {

  typedef enum {
    RESPONSE,
    POST,
    GET
  } HttpMethod;

  typedef enum {
    HTTP_OK,
    HTTP_NO_CONTENT,
    HTTP_BAD_REQUEST,
    HTTP_FORBIDDEN,
    HTTP_NOT_FOUND,
    HTTP_NOT_ACCEPTABLE,
    HTTP_TIMEOUT,
    HTTP_TOO_MANY_CONNECTIONS,
  } HttpCode;

  typedef enum {
    HTTP_CODE,
    HTTP_MESSAGE,
    HTTP_LENGTH
  } HttpCodeValue;

  const unsigned int HTTP_CODES_COUNT = 8;

  const char* const HTTP_CODES[HTTP_CODES_COUNT][2] = {
    { "200", "200 OK" },
    { "204", "204 No Content" },
    { "400", "400 Bad Request" },
    { "403", "403 Forbidden" },
    { "404", "404 Not Found" },
    { "406", "406 Not Acceptable" },
    { "408", "408 Request Timeout" },
    { "503", "503 Too Many Connections" }
  };

  const size_t HTTP_CODES_LENGTH[HTTP_CODES_COUNT] = {
    6,  // 200 OK
    14, // 204 No Content
    15, // 400 Bad Request
    13, // 403 Forbidden
    13, // 404 Not Found
    18, // 406 Not Acceptable
    19, // 408 Request Timeout
    24  // 503 Too Many Connections
  };

  typedef std::map<size_t, std::tr1::shared_ptr<base::CString> > HashedCStringMap;

} // namespace webserver

#endif // WEBSERVER_HTTPTYPES_H__
