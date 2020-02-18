#pragma once

#include <utility>
#include <variant>
#include <string>
#include <vector>
#include "SDL.hpp"
#include "TexturePath.hpp"
#include "FontDescriptor.hpp"
#include "Atlas.hpp"

namespace sg {
struct Image {
  IntRectangle rectangle;
  AtlasDescriptor atlas;
  TexturePath texture;

  Image(const IntRectangle &rectangle, AtlasDescriptor atlas, TexturePath texture)
          : rectangle(rectangle), atlas{std::move(atlas)}, texture(std::move(texture)) {}
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

  Text(FontDescriptor font, std::string text, const IntVector &position, const SDL_Color &color)
          : font(std::move(font)), text(std::move(text)), position(position), color(color) {}
};

using RenderObject = std::variant<Image, Solid, Text>;

using RenderObjectList = std::vector<RenderObject>;
}
