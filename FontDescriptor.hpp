#pragma once

#include <filesystem>

namespace sg {
struct FontDescriptor {
  std::filesystem::path path;
  unsigned size;

  bool operator<(FontDescriptor const &o) const {
    return std::make_tuple(path, size) < std::make_tuple(o.path, o.size);
  }
};
}