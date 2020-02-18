#pragma once

#include <utility>
#include <variant>
#include <string>
#include <vector>
#include "SDL.hpp"
#include "TexturePath.hpp"
#include "FontDescriptor.hpp"

namespace sg {
struct Image {
  IntRectangle rectangle;
  TexturePath texture;

  Image(const IntRectangle &rectangle, TexturePath texture) : rectangle(rectangle), texture(std::move(texture)) {}
};

struct Solid {
  IntRectangle rectangle;
  SDL_Color color;

  Solid(const IntRectangle &rectangle, const SDL_Color &color) : rectangle(rectangle), color(color) {}
};

struct Text {
  FontDescriptor font;
  std::string text;
  IntVector position;
  SDL_Color color;
};

using RenderObject = std::variant<Image, Solid, Text>;

using RenderObjectList = std::vector<RenderObject>;
}