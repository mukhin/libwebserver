// nearest_map.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

// nearest_map makes possible to do "approximate" search in map.

#ifndef BASE_NEAREST_MAP_H__
#define BASE_NEAREST_MAP_H__

#include <map>

using std::map;

namespace base {

  template<typename _Key, typename _Tp, typename _Compare = std::less<_Key>,
           typename _Alloc = std::allocator<std::pair<_Key, _Tp> > >
  class nearest_map : public map<_Key, _Tp, _Compare, _Alloc> {
  public:
    _Tp& nearest(const _Key __k) {
      // concept requirements
      __glibcxx_function_requires(_DefaultConstructibleConcept<_Tp>)

      typedef map<_Key, _Tp> parent_map;
      // Looking for the nearest suitable item.
      typename parent_map::iterator __i = parent_map::lower_bound(__k);

      // If nothing had been found, we create an item with given key.
      if (__i == parent_map::end()) {
        __i = insert(__i, typename parent_map::value_type(__k, _Tp()));
      }

      return (*__i).second;
    }
  };

} // namespace base

#endif // BASE_NEAREST_MAP_H__
