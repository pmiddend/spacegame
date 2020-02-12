#include "sdl.hpp"
#include <SDL_image.h>
#include <iostream>

namespace {
template <typename T>
SDL_Rect to_sdl_rect(sg::Rectangle<T> const &r) {
  auto const ri = sg::structure_cast<int>(r);
  return {ri.left(), ri.top(), ri.w(), ri.h()};
}

sg::IntVector get_texture_size(SDL_Texture *texture) {
  Uint32 format;
  int access, w, h;
  if (SDL_QueryTexture(texture, &format, &access, &w, &h) < 0) {
    throw std::runtime_error{"couldn't get texture information: " +
                             std::string{SDL_GetError()}};
  }
  return sg::IntVector{w, h};
}
} // namespace

sg::SDLRenderer::SDLRenderer(SDL_Renderer *const _renderer)
    : _renderer(_renderer) {}

sg::SDLRenderer::~SDLRenderer() { SDL_DestroyRenderer(_renderer); }

sg::SDLSurface::SDLSurface(SDL_Surface *const _surface) : _surface(_surface) {}

sg::SDLSurface::~SDLSurface() { SDL_FreeSurface(_surface); }

sg::SDLTexture::SDLTexture(SDL_Texture *const _texture)
    : _texture(_texture), _size(get_texture_size(_texture)) {}

sg::SDLTexture::SDLTexture(SDLTexture &&_texture)
    : _texture(_texture._texture), _size(_texture._size) {
  _texture._texture = nullptr;
}

sg::SDLSurface::SDLSurface(SDLSurface &&o) : _surface(o._surface) {
  o._surface = nullptr;
}

sg::SDLRenderer::SDLRenderer(SDLRenderer &&o) : _renderer(o._renderer) {
  o._renderer = nullptr;
}

sg::SDLTexture::~SDLTexture() { SDL_DestroyTexture(_texture); }

sg::SDLWindow::SDLWindow(SDL_Window *const _window) : _window(_window) {}

sg::SDLWindow::SDLWindow(SDLWindow &&_window) : _window(_window._window) {
  _window._window = nullptr;
}

sg::SDLWindow::~SDLWindow() { SDL_DestroyWindow(_window); }

sg::SDLRenderer sg::SDLWindow::create_renderer(IntVector const &v) {
  SDL_Renderer *const renderer{
      SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED)};
  if (renderer == nullptr)
    throw std::runtime_error{"couldn't initialize renderer: " +
                             std::string{SDL_GetError()}};
  if (SDL_RenderSetLogicalSize(renderer, v.x(), v.y()) < 0)
    throw std::runtime_error{"couldn't initialize renderer (logical size): " +
                             std::string{SDL_GetError()}};
  return SDLRenderer{renderer};
}

sg::SDLImageContext::SDLImageContext() {
  unsigned imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags))
    throw std::runtime_error{"couldn't initialize sdl image context: " +
                             std::string{IMG_GetError()}};
}

sg::SDLImageContext::~SDLImageContext() { IMG_Quit(); }

sg::SDLContext::SDLContext() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    throw std::runtime_error{"couldn't initialize SDL: " +
                             std::string{SDL_GetError()}};
}

std::optional<SDL_Event>
sg::SDLContext::wait_event(std::chrono::milliseconds const &s) {
  SDL_Event e;
  int const result{SDL_WaitEventTimeout(&e, std::max(0l, s.count()))};
  if (result == 0)
    return std::nullopt;
  return e;
}

sg::SDLContext::~SDLContext() { SDL_Quit(); }

sg::SDLWindow sg::SDLContext::create_window(IntVector const &v) {
  SDL_Window *const window{SDL_CreateWindow(
      "spacegame",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      v.x(),
      v.y(),
      SDL_WINDOW_SHOWN)};
  if (window == nullptr)
    throw std::runtime_error{"couldn't initialize window: " +
                             std::string{SDL_GetError()}};
  return SDLWindow{window};
}

sg::SDLSurface
sg::SDLImageContext::load_surface(std::filesystem::path const &p) {
  SDL_Surface *const loaded{IMG_Load(p.c_str())};
  if (loaded == nullptr)
    throw std::runtime_error{"couldn't load image \"" + std::string{p} +
                             "\": " + std::string{IMG_GetError()}};
  return SDLSurface{loaded};
}

sg::SDLTexture sg::SDLRenderer::create_texture(SDLSurface &s) {
  SDL_Texture *const texture =
      SDL_CreateTextureFromSurface(_renderer, s.surface());
  if (texture == nullptr)
    throw std::runtime_error{"couldn't convert surface to texture " +
                             std::string{SDL_GetError()}};
  return SDLTexture{texture};
}

void sg::SDLRenderer::clear() { SDL_RenderClear(_renderer); }
void sg::SDLRenderer::copy(SDLTexture &t, IntRectangle const &r) {
  auto const dest_rect = to_sdl_rect(r);
  SDL_RenderCopy(_renderer, t.texture(), nullptr, &dest_rect);
}
void sg::SDLRenderer::present() { SDL_RenderPresent(_renderer); }
