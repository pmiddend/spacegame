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
using IntVector = Vector<int>;
using DoubleVector = Vector<double>;

class SDLTexture {
public:
  explicit SDLTexture(SDL_Texture *);

  SDL_Texture *texture() { return _texture; }

  IntVector size() const { return _size; }

/*
  SDLTexture(SDLTexture &) = delete;
*/
  SDLTexture &operator=(SDLTexture const &texture) = delete;

  SDLTexture(SDLTexture &&);
  SDLTexture &operator=(SDLTexture &&);

  ~SDLTexture();
private:
  SDL_Texture *_texture;
  IntVector _size;
};

class SDLSurface {
public:
  explicit SDLSurface(SDL_Surface *);

  SDLSurface(SDLSurface &&);

  SDL_Surface *surface() { return _surface; }

  ~SDLSurface();

private:
  SDL_Surface *_surface;
};

using IntRectangle = Rectangle<int>;

class SDLRenderer {
public:
  explicit SDLRenderer(SDL_Renderer *);

  SDLRenderer(SDLRenderer &&);

  SDLTexture create_texture(SDLSurface &);
  void clear();
  void copy_whole(SDLTexture &, IntRectangle const &);
  void copy(SDLTexture &, IntRectangle const &from, IntRectangle const &to);
  void present();

  ~SDLRenderer();

private:
  SDL_Renderer *_renderer;
};

class SDLWindow {
public:
  explicit SDLWindow(SDL_Window *);

  ~SDLWindow();

  SDLRenderer create_renderer(IntVector const &);

  SDLWindow(SDLWindow &&w);

private:
  SDL_Window *_window;
};

class SDLImageContext {
  SDLImageContext(SDLImageContext const &) = delete;
  SDLImageContext &operator=(SDLImageContext const &) = delete;

public:
  SDLImageContext();

  SDLSurface load_surface(std::filesystem::path const &);

  ~SDLImageContext();
};

class SDLContext {
  SDLContext(SDLContext const &) = delete;
  SDLContext &operator=(SDLContext const &) = delete;

public:
  SDLContext();
  ~SDLContext();

  std::optional<SDL_Event> wait_event(std::chrono::milliseconds const &);

  SDLWindow create_window(IntVector const &);
};

class SDLMixerChunk;

class SDLMixerContext {
public:
    SDLMixerContext(SDLContext const &);
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
  SDLMixerChunk(Mix_Chunk *);
  SDLMixerChunk(SDLMixerChunk &&);
  Mix_Chunk *chunk() { return chunk_; }
  ~SDLMixerChunk();
private:
  Mix_Chunk *chunk_;
};

class SDLTTFFont;

class SDLTTFContext {
public:
  SDLTTFContext();
  SDLTTFFont open_font(std::filesystem::path const &, unsigned size);
  ~SDLTTFContext();
};

class SDLTTFFont {
public:
  SDLTTFFont(TTF_Font *);
  SDLTTFFont(SDLTTFFont &&);
  TTF_Font *font() { return font_; }
  SDLSurface render_blended(std::string const &, SDL_Color const &);
  ~SDLTTFFont();
private:
  TTF_Font *font_;
};
} // namespace sg
