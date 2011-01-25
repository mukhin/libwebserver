// functional.h
// Asyncronous I/O library.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved. 

#include <functional>

namespace io {

  template <typename Type, typename Ftor>
  struct equal_t {
    typedef bool result_type;

    equal_t(Type t, Ftor f) : m_t(t), m_f(f) {}

    template <typename Arg>
    bool operator () (Arg& a) {
      return m_t == m_f(a);
    }

    Type m_t;
    Ftor m_f;
  };

  template <typename Type, typename Ftor>
  inline equal_t<Type, Ftor>
  equal(Type t, Ftor f) {
    return equal_t<Type, Ftor>(t, f);
  }

  template <typename Class, typename Member>
  struct mem_ref_t : public std::unary_function<Class&, Member&> {
    mem_ref_t(Member Class::*m) : m_member(m) {}

    Member& operator () (Class& c) {
      return c.*m_member;
    }

    Member Class::*m_member;
  };

  template <typename Class, typename Member>
  inline mem_ref_t<Class, Member>
  mem_ref(Member Class::*m) {
    return mem_ref_t<Class, Member>(m);
  }

} // namespace io
