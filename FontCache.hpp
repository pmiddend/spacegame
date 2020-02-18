#pragma once

#include <map>
#include <filesystem>
#include "SDL.hpp"
#include "FontDescriptor.hpp"
#include "util.hpp"
#include "types.hpp"
#include "lru.hpp"

namespace sg {
struct TextDescriptor {
  FontDescriptor font;
  std::string text;
  Color color;

  bool operator<(TextDescriptor const &o) const {
    return std::make_tuple(font, text) < std::make_tuple(o.font, o.text);
  }
};

class FontCache {
private:
  using FontMap = std::map<FontDescriptor, SDLTTFFont>;
  using TextMap = LRU<TextDescriptor, SDLTexture>;

public:
  FontCache(SDLTTFContext &, SDLRenderer &);

  void copy_text(FontDescriptor const &, std::string const &, Color const &, IntVector const &);

private:
  sg::SDLTTFContext &font_context_;
  sg::SDLRenderer &renderer_;
  FontMap fonts_;
  TextMap texts_;

  sg::SDLTexture &
  render_text(FontDescriptor const &, std::string const &, Color const &);
};
}

