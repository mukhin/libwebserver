// equal.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef BASE_EQUALS_H__
#define BASE_EQUALS_H__

#include <ext/rope>
#include <tr1/memory>


namespace base {

  template<typename T>
  class Equal : public std::binary_function<T, T, bool> {
  public:
    static bool equal(const T& x, const T& y) {
      return x == y;
    }

    bool operator()(const T& x, const T& y) const {
      return x == y;
    }
  };


  template<>
  class Equal<const char*> : public std::binary_function<const char*, const char*, bool> {
  public:
    static bool equal(const char* x, const char* y) {
      return ::strcmp(x, y) == 0;
    }

    bool operator()(const char* x, const char* y) const {
      return equal(x, y);
    }
  };


  template<>
  class Equal<std::string> : public std::binary_function<std::string, std::string, bool> {
  public:
    static bool equal(const std::string& x, const std::string& y) {
      return x == y;
    }

    static bool equal(const std::string& x, const char* y) {
      return ::strcmp(x.c_str(), y) == 0;
    }

    static bool equal(const char* x, const std::string& y) {
      return ::strcmp(x, y.c_str()) == 0;
    }

    bool operator()(const std::string& x, const std::string& y) const {
      return x == y;
    }

    bool operator()(const std::string& x, const char* y) const {
      return equal(x, y);
    }

    bool operator()(const char* x, const std::string& y) const {
      return equal(x, y);
    }
  };


  template<>
  class Equal<std::tr1::shared_ptr<std::string> >
  : public std::binary_function<std::tr1::shared_ptr<std::string>, std::tr1::shared_ptr<std::string>, bool> {
  public:
    static bool equal(const std::tr1::shared_ptr<std::string>& x, const std::tr1::shared_ptr<std::string>& y) {
      return *x == *y;
    }

    bool operator()(const std::tr1::shared_ptr<std::string>& x, const std::tr1::shared_ptr<std::string>& y) const {
      return *x == *y;
    }
  };


  template<>
  class Equal<__gnu_cxx::crope> : public std::binary_function<__gnu_cxx::crope, __gnu_cxx::crope, bool> {
  public:
    static bool equal(const __gnu_cxx::crope& x, const __gnu_cxx::crope& y) {
      return x == y;
    }

    bool operator()(const __gnu_cxx::crope& x, const __gnu_cxx::crope& y) const {
      return x == y;
    }
  };


  template<>
  class Equal<std::tr1::shared_ptr<__gnu_cxx::crope> >
  : public std::binary_function<std::tr1::shared_ptr<__gnu_cxx::crope>, std::tr1::shared_ptr<__gnu_cxx::crope>, bool> {
  public:
    static bool equal(const std::tr1::shared_ptr<__gnu_cxx::crope>& x,
                      const std::tr1::shared_ptr<__gnu_cxx::crope>& y) {
      return *x == *y;
    }

    bool operator()(const std::tr1::shared_ptr<__gnu_cxx::crope>& x,
                    const std::tr1::shared_ptr<__gnu_cxx::crope>& y) const {
      return *x == *y;
    }
  };

} // namespace base

#endif // BASE_EQUALS_H__
