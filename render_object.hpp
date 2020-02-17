#pragma once

#include <utility>
#include <variant>
#include <string>
#include <vector>
#include "sdl.hpp"
#include "texture_path.hpp"

namespace sg {
struct Image {
  IntRectangle rectangle;
  texture_path texture;

  Image(const IntRectangle &rectangle, texture_path texture) : rectangle(rectangle), texture(std::move(texture)) {}
};

struct Solid {
  IntRectangle rectangle;
  SDL_Color color;

  Solid(const IntRectangle &rectangle, const SDL_Color &color) : rectangle(rectangle), color(color) {}
};

struct Text {
  std::string text;
};

using RenderObject = std::variant<Image, Solid, Text>;

using RenderObjectList = std::vector<RenderObject>;
}
