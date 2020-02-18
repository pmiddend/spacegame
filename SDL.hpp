#pragma once

#include "math.hpp"
#include <SDL.h>
#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

namespace sg {
class SDLTexture {
public:
  explicit SDLTexture(SDL_Texture *);

  SDL_Texture *texture() { return _texture; }

  [[nodiscard]] IntVector size() const { return _size; }

  SDLTexture(SDLTexture &) = delete;

  SDLTexture &operator=(SDLTexture const &texture) = delete;

  SDLTexture(SDLTexture &&) noexcept;

  SDLTexture &operator=(SDLTexture &&) noexcept;

  ~SDLTexture();

private:
  SDL_Texture *_texture;
  IntVector _size;
};

class SDLSurface {
public:
  explicit SDLSurface(SDL_Surface *);

  SDLSurface(SDLSurface &) = delete;

  SDLSurface &operator=(SDLSurface const &) = delete;

  SDLSurface(SDLSurface &&) noexcept;

  SDLSurface &operator=(SDLSurface &&) noexcept;

  SDL_Surface *surface() { return _surface; }

  ~SDLSurface();

private:
  SDL_Surface *_surface;
};

class SDLRenderer {
public:
  explicit SDLRenderer(SDL_Renderer *);

  SDLRenderer(SDLRenderer &) = delete;

  SDLRenderer &operator=(SDLRenderer const &) = delete;

  SDLRenderer(SDLRenderer &&) = delete;

  SDLRenderer &operator=(SDLRenderer &&) = delete;

  SDLTexture create_texture(SDLSurface &);

  void clear();

  void copy_whole(SDLTexture &, IntRectangle const &);

  void copy(SDLTexture &, IntRectangle const &from, IntRectangle const &to);

  void present();

  void fill_rect(IntRectangle const &, SDL_Color const &);

  ~SDLRenderer();

private:
  SDL_Renderer *_renderer;
};

class SDLWindow {
public:
  explicit SDLWindow(SDL_Window *);

  SDLWindow(SDLWindow &) = delete;

  SDLWindow &operator=(SDLWindow const &) = delete;

  SDLWindow(SDLWindow &&) = delete;

  SDLWindow &operator=(SDLWindow &&) = delete;

  ~SDLWindow();

  SDLRenderer create_renderer(IntVector const &);

private:
  SDL_Window *_window;
};

class SDLImageContext {
public:
  SDLImageContext(SDLImageContext const &) = delete;

  SDLImageContext &operator=(SDLImageContext const &) = delete;

  SDLImageContext(SDLImageContext &&) = delete;

  SDLImageContext &operator=(SDLImageContext &&) = delete;

  SDLImageContext();

  SDLSurface load_surface(std::filesystem::path const &);

  ~SDLImageContext();
};

class SDLContext {
public:
  SDLContext(SDLContext const &) = delete;

  SDLContext &operator=(SDLContext const &) = delete;

  SDLContext(SDLContext &&) = delete;

  SDLContext &operator=(SDLContext &&) = delete;

  SDLContext();

  ~SDLContext();

  std::optional<SDL_Event> wait_event(std::chrono::milliseconds const &);

  SDLWindow create_window(IntVector const &);
};

class SDLMixerChunk;

class SDLMixerContext {
public:
  SDLMixerContext(SDLMixerContext const &) = delete;

  SDLMixerContext &operator=(SDLMixerContext const &) = delete;

  SDLMixerContext(SDLMixerContext &&) = delete;

  SDLMixerContext &operator=(SDLMixerContext &&) = delete;

  explicit SDLMixerContext(SDLContext const &);

  ~SDLMixerContext();

  void play_music(std::filesystem::path const &);

  SDLMixerChunk load_chunk(std::filesystem::path const &);

  void play_chunk(SDLMixerChunk &);

private:
  bool lib_inited_;
  Mix_Music *music_;
};

class SDLMixerChunk {
public:
  explicit SDLMixerChunk(Mix_Chunk *);

  SDLMixerChunk(SDLMixerChunk const &) = delete;

  SDLMixerChunk &operator=(SDLMixerChunk const &) = delete;

  SDLMixerChunk(SDLMixerChunk &&) noexcept;

  SDLMixerChunk &operator=(SDLMixerChunk &&) noexcept;

  Mix_Chunk *chunk() { return chunk_; }

  ~SDLMixerChunk();

private:
  Mix_Chunk *chunk_;
};

class SDLTTFFont;

class SDLTTFContext {
public:
  SDLTTFContext();

  SDLTTFContext(SDLTTFContext const &) = delete;

  SDLTTFContext &operator=(SDLTTFContext const &) = delete;

  SDLTTFContext(SDLTTFContext &&) = delete;

  SDLTTFContext &operator=(SDLTTFContext &&) = delete;

  SDLTTFFont open_font(std::filesystem::path const &, unsigned size);

  ~SDLTTFContext();
};

class SDLTTFFont {
public:
  explicit SDLTTFFont(TTF_Font *);

  SDLTTFFont(SDLTTFFont const &) = delete;

  SDLTTFFont &operator=(SDLTTFFont const &) = delete;

  SDLTTFFont(SDLTTFFont &&) noexcept;

  SDLTTFFont &operator=(SDLTTFFont &&) noexcept;

  SDLSurface render_blended(std::string const &, SDL_Color const &);

  ~SDLTTFFont();

private:
  TTF_Font *font_;
};
} // namespace sg
