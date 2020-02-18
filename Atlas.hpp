#pragma once

#include <map>
#include <string>
#include <filesystem>
#include "TextureCache.hpp"
#include "TexturePath.hpp"

namespace sg {
class Atlas {
public:
  using AtlasMap = std::map<std::string, IntRectangle>;
  Atlas(TextureCache &textures, std::filesystem::path);

  void render_tile(SDLRenderer &renderer, TexturePath const &, IntRectangle const &) const;
private:
  SDLTexture &texture_;
  AtlasMap atlas_;
};
}

