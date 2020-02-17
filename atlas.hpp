#pragma once

#include <map>
#include <string>
#include <filesystem>
#include "texture_cache.hpp"
#include "texture_path.hpp"

namespace sg {
class Atlas {
public:
  using AtlasMap = std::map<std::string, IntRectangle>;
  Atlas(TextureCache &textures, std::filesystem::path);

  void render_tile(SDLRenderer &renderer, texture_path const &, IntRectangle const &) const;
private:
  SDLTexture &texture_;
  AtlasMap atlas_;
};
}

