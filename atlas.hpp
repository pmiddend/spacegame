#pragma once

#include <map>
#include <string>
#include <filesystem>
#include "texture_cache.hpp"

class Atlas {
public:
  using AtlasMap = std::map<std::string, sg::IntRectangle>;
  Atlas(TextureCache &textures, std::filesystem::path);

  void render_tile(sg::SDLRenderer &renderer, std::string const &, sg::IntRectangle const &) const;
private:
  sg::SDLTexture &texture_;
  AtlasMap atlas_;
};

