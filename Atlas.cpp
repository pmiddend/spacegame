#include "Atlas.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <utility>

sg::Atlas sg::Atlas::from_json(TextureCache &textures, std::filesystem::path const &png_file) {
  AtlasMap atlas_;
  auto const json_path = std::filesystem::path(png_file).replace_extension(".json");
  std::ifstream json_file{json_path};
  nlohmann::json atlas_json;
  json_file >> atlas_json;
  auto const frames = atlas_json.find("frames");
  if (frames == atlas_json.end())
    throw std::runtime_error{"couldn't find \"frames\" in " + json_path.string()};
  for (const auto &el : frames->items()) {
    auto const frame = el.value().find("frame");
    if (frame == el.value().end())
      throw std::runtime_error{R"(couldn't find "frame" inside ")" + el.key() + "\" inside " + json_path.string()};
    atlas_.insert(AtlasMap::value_type{el.key(),
                                       sg::IntRectangle::from_pos_and_size(sg::IntVector{frame->at("x").get<int>(),
                                                                                         frame->at("y")},
                                                                           sg::IntVector{frame->at("w"),
                                                                                         frame->at("h")})});
  }
  return Atlas{textures.get_texture(png_file), atlas_};
}

void sg::Atlas::render_tile(sg::SDLRenderer &renderer, TexturePath const &tile, const sg::IntRectangle &to) const {
  renderer.copy(*texture_, atlas_.at(tile.path), to);
}

sg::Atlas::Atlas(sg::Atlas &&o) noexcept: texture_(o.texture_), atlas_(std::move(o.atlas_)) {

}

sg::Atlas &sg::Atlas::operator=(sg::Atlas &&o) noexcept {
  std::swap(texture_, o.texture_);
  atlas_.swap(o.atlas_);
  return *this;
}

sg::Atlas::Atlas(sg::SDLTexture &_texture, sg::Atlas::AtlasMap _atlas)
        : texture_{&_texture}, atlas_{std::move(_atlas)} {
}

sg::Atlas
sg::Atlas::from_animation(TextureCache &textures, std::filesystem::path const &p, IntVector const &tile_size) {
  SDLTexture &texture{textures.get_texture(p)};
  int const per_row{texture.size().x() / tile_size.x()};
  int const rows{texture.size().y() / tile_size.y()};
  AtlasMap atlas;
  for (int i{0}; i < per_row * rows; ++i) {
    auto const pos{sg::IntVector{static_cast<int>(i % per_row), static_cast<int>(i / per_row)} * tile_size};
    atlas.insert(AtlasMap::value_type{std::to_string(i), IntRectangle::from_pos_and_size(pos, tile_size)});
  }
  return Atlas{textures.get_texture(p), atlas};
}

sg::AtlasCache::AtlasCache(TextureCache &_textures) noexcept: textures_{_textures}, atlases_{} {

}

sg::Atlas &sg::AtlasCache::get(const sg::AtlasDescriptor &d) {
  for (AtlasPair &p : this->atlases_)
    if (p.first == d)
      return p.second;
  Atlas new_atlas{d.tile_size.has_value() ? Atlas::from_animation(this->textures_, d.path, d.tile_size.value())
                                          : Atlas::from_json(this->textures_, d.path)};
  this->atlases_.push_back(AtlasPair{d, std::move(new_atlas)});
  return this->atlases_.back().second;
}
