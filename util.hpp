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

template<typename T>
void append(T &a, T const &b) {
  a.insert(a.end(), b.begin(), b.end());
}

template<typename T, typename U>
auto promoting_min(T const &t, U const &u) -> decltype(t+u) {
  using TargetType = decltype(t+u);
  return std::min(static_cast<TargetType>(t), static_cast<TargetType>(u));
}

#define SG_NONCOPYABLE(ClassName) ClassName(ClassName const &) = delete; ClassName &operator=(ClassName const &) = delete
#define SG_NONMOVEABLE(ClassName) ClassName(ClassName &&) = delete; ClassName &operator=(ClassName &&) = delete
}