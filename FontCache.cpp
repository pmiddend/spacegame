#include "FontCache.hpp"

sg::FontCache::FontCache(SDLTTFContext &_font_context, SDLRenderer &_renderer)
        : font_context_{_font_context}, renderer_{_renderer}, fonts_{}, texts_{64} {}


void sg::FontCache::copy_text(FontDescriptor const &font, std::string const &text, Color const &color,
                              IntVector const &position) {
  SDLTexture &texture{this->render_text(font, text, color)};
  auto const text_size{texture.size()};
  renderer_.copy_whole(texture,
                       IntRectangle::from_pos_and_size(position, text_size));
}

sg::SDLTexture &
sg::FontCache::render_text(FontDescriptor const &font, std::string const &text, Color const &color) {
  TextDescriptor const tdescriptor{font, text, color};
  if (this->texts_.exists(tdescriptor))
    return this->texts_.get(tdescriptor);
  SDLTTFFont &existing_font{map_insert_or_load(this->fonts_,
                                               font,
                                               [this, &font]() {
                                                 return font_context_.open_font(font.path,
                                                                                font.size);
                                               })};
  SDLSurface surface{existing_font.render_blended(text, color)};
  SDLTexture texture{this->renderer_.create_texture(surface)};
  return texts_.put(tdescriptor, std::move(texture));
}
