#pragma once

#include <map>
#include <filesystem>
#include "sdl.hpp"

class TextureCache {
private:
    using TextureMap = std::map<std::filesystem::path, sg::SDLTexture>;

public:
    TextureCache(sg::SDLImageContext &image_context, sg::SDLRenderer &renderer)
            : _image_context{image_context}, _renderer{renderer} {}

    sg::SDLTexture &get_texture(std::filesystem::path const &p) {
        TextureMap::iterator it{_textures.find(p)};
        if (it != _textures.end()) {
            return it->second;
        }
        auto surface = _image_context.load_surface(p);
        return _textures
                .insert(TextureMap::value_type{p, _renderer.create_texture(surface)})
                .first->second;
    }

private:
    sg::SDLImageContext &_image_context;
    sg::SDLRenderer &_renderer;
    TextureMap _textures;
};

