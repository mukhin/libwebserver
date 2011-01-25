// containers.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef BASE_CONTAINERS_H__
#define BASE_CONTAINERS_H__

#include "hash.h"
#include "istring.h"
#include "string_helpers.h"

#include <iterator>
#include <google/dense_hash_map>

#undef _END_GOOGLE_NAMESPACE_
#undef _START_GOOGLE_NAMESPACE_

#include <google/dense_hash_set>

namespace base {

  template<class K, K empty, K deleted>
  class dense_hash_set : public google::dense_hash_set<K, Hasher<K>, Hasher<K> > {
  public:
    typedef K KeyType;

    dense_hash_set()
    : empty_(empty)
    , deleted_(deleted) {
      set_empty_key(empty);
      set_deleted_key(deleted);
    }

    bool IsEmptyKey(const K& key) {
      return key == empty_ || key == deleted_;
    }

  private:
    K empty_;
    K deleted_;
  };


  template<class K, class V, K empty, K deleted>
  class dense_hash_map : public google::dense_hash_map<K, V, Hasher<K>, Hasher<K> > {
  public:
    typedef K KeyType;
    typedef V ValueType;

    dense_hash_map()
    : empty_(empty)
    , deleted_(deleted) {
      set_empty_key(empty);
      set_deleted_key(deleted);
    }

    bool IsEmptyKey(const K& key) {
      return key == empty_ || key == deleted_;
    }

  private:
    K empty_;
    K deleted_;
  };

} // namespace base

#endif // BASE_CONTAINERS_H__
