#pragma once

#include "math.hpp"
#include <SDL.h>
#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>

namespace sg {
using IntVector = Vector<int>;
using DoubleVector = Vector<double>;

class SDLTexture {
public:
  explicit SDLTexture(SDL_Texture *);

  SDLTexture(SDLTexture &&);

  SDL_Texture *texture() { return _texture; }

  IntVector size() const { return _size; }

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
  void copy(SDLTexture &, IntRectangle const &);
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
} // namespace sg
