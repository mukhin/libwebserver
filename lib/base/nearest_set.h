// nearest_set.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.
 
// nearest_set makes possible to do "approximate" search in set.

#ifndef BASE_NEAREST_SET_H__
#define BASE_NEAREST_SET_H__

#include <set>

using std::set;

namespace base {

  template<class _Key, class _Compare = std::less<const _Key>, class _Alloc = std::allocator<_Key> >
  class nearest_set : public set<_Key, _Compare, _Alloc> {
  public:
    _Key& operator[] (const _Key __k) {
      typedef set<_Key> parent_set;
      // Looking for the nearest suitable item.
      typename parent_set::iterator __i = parent_set::lower_bound(__k);

      // If nothing had been found, we create an item with given key.
      if (__i == parent_set::end()) {
        __i = insert(__k);
      }

      return (*__i);
    }
  };

} // namespace base

#endif // BASE_NEAREST_SET_H__
