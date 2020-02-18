#pragma once

#include <map>
#include <list>
#include <cstddef>
#include <stdexcept>

namespace sg {

template<typename K, typename V>
class LRU {
public:
  using key_value_pair_t = std::pair<K, V>;
  using list_iterator_t = typename std::list<key_value_pair_t>::iterator;

  explicit LRU(std::size_t const max_size) : _max_size{max_size} {
  }

  V &put(const K &key, V &&value) {
    auto it{_cache_items_map.find(key)};
    _cache_items_list.push_front(key_value_pair_t{  key, std::move(value)  });
    if (it != _cache_items_map.end()) {
      _cache_items_list.erase(it->second);
      _cache_items_map.erase(it);
    }
    _cache_items_map[key] = _cache_items_list.begin();

    if (_cache_items_map.size() > _max_size) {
      auto last{_cache_items_list.end()};
      last--;
      _cache_items_map.erase(last->first);
      _cache_items_list.pop_back();
    }
    return _cache_items_list.begin()->second;
  }

  V &get(const K &key) {
    auto it{_cache_items_map.find(key)};
    if (it == _cache_items_map.end())
      throw std::range_error("There is no such key in cache");
    _cache_items_list.splice(_cache_items_list.begin(), _cache_items_list, it->second);
    return it->second->second;
  }

  bool exists(const K &key) const {
    return _cache_items_map.find(key) != _cache_items_map.end();
  }

  [[nodiscard]] std::size_t size() const {
    return _cache_items_map.size();
  }

private:
  std::list<key_value_pair_t> _cache_items_list;
  std::map<K, list_iterator_t> _cache_items_map;
  size_t _max_size;
};
}
