// hash.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef BASE_HASH_H__
#define BASE_HASH_H__

#include <ext/rope>
#include <tr1/memory>


namespace base {

  template<typename T>
  class Hash : public std::unary_function<T, size_t> {
  public:
    static size_t hash(const T& x) {
      return x;
    }

    size_t operator()(const T& x) const {
      return x;
    }
  };


  template<typename T>
  class Less {
  public:
    bool operator()(const T& x, const T& y) const {
      return x < y;
    }
  };


  template<>
  class Hash<const char*> : public std::unary_function<const char*, size_t>{
  public:
    static size_t hash(const char* x) {
      size_t h = 0;
      for (const char* s = x; *s; ++s) {
        h = (h * 17) ^ *s;
      }
      return h;
    }

    size_t operator()(const char* x) const {
      return hash(x);
    }
  };


  template<>
  class Hash<std::string> : public std::unary_function<std::string, size_t> {
  public:
    static size_t hash(const std::string& x) {
      size_t h = 0;
      for (const char* s = x.c_str(); *s; ++s) {
        h = (h * 17) ^ *s;
      }
      return h;
    }

    size_t operator()(const std::string& x) const {
      return hash(x);
    }
  };


  template<>
  class Hash<std::tr1::shared_ptr<std::string> >
  : public std::unary_function<std::tr1::shared_ptr<std::string>, size_t> {
  public:
    static size_t hash(const std::tr1::shared_ptr<std::string>& x) {
      size_t h = 0;
      for (const char* s = x->c_str(); *s; ++s) {
        h = (h * 17) ^ *s;
      }
      return h;
    }

    size_t operator()(const std::tr1::shared_ptr<std::string>& x) const {
      return hash(x);
    }
  };


  template<>
  class Hash<__gnu_cxx::crope> : public std::unary_function<__gnu_cxx::crope, size_t> {
  public:
    static size_t hash(const __gnu_cxx::crope& x) {
      size_t h = 0;
      for (const char* s = x.c_str(); *s; ++s) {
        h = (h * 17) ^ *s;
      }
      return h;
    }

    size_t operator()(const __gnu_cxx::crope& x) const {
      return hash(x);
    }
  };


  template<>
  class Hash<std::tr1::shared_ptr<__gnu_cxx::crope> >
  : public std::unary_function<std::tr1::shared_ptr<__gnu_cxx::crope>, size_t> {
  public:
    static size_t hash(const std::tr1::shared_ptr<__gnu_cxx::crope>& x) {
      size_t h = 0;
      for (const char* s = x->c_str(); *s; ++s) {
        h = (h * 17) ^ *s;
      }
      return h;
    }

    size_t operator()(const std::tr1::shared_ptr<__gnu_cxx::crope>& x) const {
      return hash(x);
    }
  };

} // namespace base

#endif // BASE_HASH_H__
