#pragma once

#include <variant>
#include <string>
#include "sdl.hpp"

struct Sprite {
  IntRectangle rectangle;
  SDLTexture &texture;
};

struct Rectangle {
  IntRectangle rectangle;
};

struct Text {
  std::string text;
};

using RenderObject = std::variant<>;
