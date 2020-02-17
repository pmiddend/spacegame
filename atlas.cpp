#include "atlas.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

sg::Atlas::Atlas(TextureCache &textures, std::filesystem::path f) : texture_(textures.get_texture(f)), atlas_{} {
  auto const json_path = f.replace_extension(".json");
  std::ifstream json_file{json_path};
  nlohmann::json atlas_json;
  json_file >> atlas_json;
  auto const frames = atlas_json.find("frames");
  if (frames == atlas_json.end())
    throw std::runtime_error{"couldn't find \"frames\" in " + json_path.string()};
  for (auto el : frames->items()) {
    auto const frame = el.value().find("frame");
    if (frame == el.value().end())
      throw std::runtime_error{"couldn't find \"frame\" inside \"" + el.key() + "\" inside " + json_path.string()};
    atlas_.insert(
            AtlasMap::value_type{el.key(), sg::IntRectangle::from_pos_and_size(
                    sg::IntVector{frame->at("x").get<int>(), frame->at("y")},
                    sg::IntVector{frame->at("w"), frame->at("h")})});
  }
}

void sg::Atlas::render_tile(sg::SDLRenderer &renderer, const std::string &tile, const sg::IntRectangle &to) const {
  renderer.copy(texture_, atlas_.at(tile), to);
}
