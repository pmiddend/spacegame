#pragma once

#include <map>
#include <string>
#include <filesystem>
#include <utility>
#include <vector>
#include "TextureCache.hpp"
#include "TexturePath.hpp"

namespace sg {
class AtlasDescriptor {
public:
  std::filesystem::path path;
  std::optional<IntVector> tile_size;

  AtlasDescriptor(std::filesystem::path path, const std::optional<IntVector> &tile_size)
          : path{std::move(path)}, tile_size{tile_size} {}

  bool operator==(AtlasDescriptor const &o) const {
    return path == o.path && tile_size == o.tile_size;
  }
};

class Atlas {
public:
  using AtlasMap = std::map<std::string, IntRectangle>;

  Atlas(SDLTexture &, AtlasMap);

  Atlas(Atlas const &) = delete;

  Atlas &operator=(Atlas const &) = delete;

  Atlas(Atlas &&) noexcept;

  Atlas &operator=(Atlas &&) noexcept;

  static Atlas from_animation(TextureCache &textures, std::filesystem::path const &, IntVector const &tile_size);

  static Atlas from_json(TextureCache &textures, std::filesystem::path const &);

  void render_tile(SDLRenderer &renderer, TexturePath const &, IntRectangle const &) const;

private:
  SDLTexture *texture_;
  AtlasMap atlas_;
};

class AtlasCache {
public:
  explicit AtlasCache(TextureCache &) noexcept;

  Atlas &get(AtlasDescriptor const &);

  AtlasCache &operator=(AtlasCache const &) = delete;

  AtlasCache(AtlasCache const &) = delete;

  AtlasCache &operator=(AtlasCache &&) = delete;

  AtlasCache(AtlasCache &&) = delete;

private:
  using AtlasPair = std::pair<AtlasDescriptor, Atlas>;
  TextureCache &textures_;
  std::vector<AtlasPair> atlases_;
};
}

