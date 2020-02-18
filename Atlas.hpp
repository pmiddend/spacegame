#pragma once

#include <map>
#include <string>
#include <filesystem>
#include <utility>
#include <vector>
#include "TextureCache.hpp"
#include "TexturePath.hpp"

namespace sg {
using AnimationDuration = std::chrono::milliseconds;

class AnimationDescriptor {
public:
  IntVector tile_size;
  unsigned tile_count;
  AnimationDuration duration;

  AnimationDescriptor(const IntVector &tile_size, unsigned int tile_count, AnimationDuration const &duration)
          : tile_size{tile_size}, tile_count{tile_count}, duration{duration} {}

  bool operator==(AnimationDescriptor const &o) const {
    return tile_size == o.tile_size && tile_count == o.tile_count && duration == o.duration;
  }
};

class AtlasDescriptor {
public:
  std::filesystem::path path;
  std::optional<AnimationDescriptor> animation;

  AtlasDescriptor(std::filesystem::path path, const std::optional<AnimationDescriptor> &animation)
          : path{std::move(path)}, animation{animation} {}

  bool operator==(AtlasDescriptor const &o) const {
    return path == o.path && animation == o.animation;
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

  static Atlas from_descriptor(TextureCache &textures, AtlasDescriptor const &);

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

