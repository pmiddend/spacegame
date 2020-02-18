#pragma once

#include <map>

namespace sg {
template<typename K, typename V, typename F>
V &map_insert_or_load(std::map<K, V> &m, K const &k, F const &f) {
  auto existing_font = m.find(k);
  if (existing_font != m.end())
    return existing_font->second;
  return m.insert(typename std::map<K, V>::value_type{k,
                                                      f()}).first->second;
}
}