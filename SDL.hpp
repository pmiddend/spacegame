#pragma once

#include "math.hpp"
#include "util.hpp"
#include <SDL.h>
#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>

namespace sg {
class SDLTexture {
public:
  explicit SDLTexture(SDL_Texture *);

  SDL_Texture *texture() { return _texture; }

  [[nodiscard]] IntVector size() const { return _size; }

  SG_NONCOPYABLE(SDLTexture);

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

  SG_NONCOPYABLE(SDLSurface);

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

  SG_NONCOPYABLE(SDLRenderer); SG_NONMOVEABLE(SDLRenderer);

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

  SG_NONCOPYABLE(SDLWindow); SG_NONMOVEABLE(SDLWindow);

  ~SDLWindow();

  SDLRenderer create_renderer(IntVector const &);

private:
  SDL_Window *_window;
};

class SDLImageContext {
public: SG_NONCOPYABLE(SDLImageContext); SG_NONMOVEABLE(SDLImageContext);

  SDLImageContext();

  SDLSurface load_surface(std::filesystem::path const &);

  ~SDLImageContext();
};

class SDLContext {
public: SG_NONCOPYABLE(SDLContext); SG_NONMOVEABLE(SDLContext);

  SDLContext();

  ~SDLContext();

  std::vector<SDL_Event> wait_event(std::chrono::milliseconds const &);

  SDLWindow create_window(IntVector const &);
};

class SDLMixerChunk;

class SDLMixerContext {
public: SG_NONCOPYABLE(SDLMixerContext); SG_NONMOVEABLE(SDLMixerContext);

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
public: SG_NONCOPYABLE(SDLMixerChunk);

  explicit SDLMixerChunk(Mix_Chunk *);

  SDLMixerChunk(SDLMixerChunk &&) noexcept;

  SDLMixerChunk &operator=(SDLMixerChunk &&) noexcept;

  Mix_Chunk *chunk() { return chunk_; }

  ~SDLMixerChunk();

private:
  Mix_Chunk *chunk_;
};

class SDLTTFFont;

class SDLTTFContext {
public: SG_NONCOPYABLE(SDLTTFContext); SG_NONMOVEABLE(SDLTTFContext);

  SDLTTFContext();

  SDLTTFFont open_font(std::filesystem::path const &, unsigned size);

  ~SDLTTFContext();
};

class SDLTTFFont {
public: SG_NONCOPYABLE(SDLTTFFont);

  explicit SDLTTFFont(TTF_Font *);

  SDLTTFFont(SDLTTFFont &&) noexcept;

  SDLTTFFont &operator=(SDLTTFFont &&) noexcept;

  SDLSurface render_blended(std::string const &, SDL_Color const &);

  ~SDLTTFFont();

private:
  TTF_Font *font_;
};
} // namespace sg
