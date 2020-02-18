#pragma once

#include <map>
#include <filesystem>
#include "SDL.hpp"
#include "util.hpp"

namespace sg {
class TextureCache {
private:
  using TextureMap = std::map<std::filesystem::path, sg::SDLTexture>;

public:
  TextureCache(sg::SDLImageContext &image_context, sg::SDLRenderer &renderer)
          : image_context_{image_context}, renderer_{renderer} {}

  sg::SDLTexture &get_texture(std::filesystem::path const &p) {
    TextureMap::iterator it{textures_.find(p)};
    if (it != textures_.end()) {
      return it->second;
    }
    auto surface = image_context_.load_surface(p);
    return textures_.insert(TextureMap::value_type{p, renderer_.create_texture(surface)}).first->second;
  }

  SG_NONCOPYABLE(TextureCache);
  SG_NONMOVEABLE(TextureCache);

private:
  sg::SDLImageContext &image_context_;
  sg::SDLRenderer &renderer_;
  TextureMap textures_;
};
}

