#include "SDL.hpp"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <thread>

namespace {
std::string sdl_error_string() {
  return std::string{SDL_GetError()};
}

template<typename T>
SDL_Rect to_sdl_rect(sg::Rectangle<T> const &r) {
  auto const ri = sg::structure_cast<int>(r);
  return {ri.left(), ri.top(), ri.w(), ri.h()};
}

sg::IntVector get_texture_size(SDL_Texture *texture) {
  Uint32 format;
  int access, w, h;
  if (SDL_QueryTexture(texture, &format, &access, &w, &h) < 0) {
    throw std::runtime_error{"couldn't get texture information: " +
                             sdl_error_string()};
  }
  return sg::IntVector{w, h};
}

} // namespace

sg::SDLRenderer::SDLRenderer(SDL_Renderer *const _renderer)
        : _renderer(_renderer) {
  if (SDL_SetRenderDrawBlendMode(this->_renderer, SDL_BLENDMODE_BLEND) != 0)
    throw std::runtime_error{"couldn't set blend mode: " +
                             sdl_error_string()};
}

sg::SDLRenderer::~SDLRenderer() { SDL_DestroyRenderer(_renderer); }

sg::SDLSurface::SDLSurface(SDL_Surface *const _surface) : _surface(_surface) {}

sg::SDLSurface::~SDLSurface() { SDL_FreeSurface(_surface); }

sg::SDLTexture::SDLTexture(SDL_Texture *const _texture)
        : _texture(_texture), _size(get_texture_size(_texture)) {}

sg::SDLTexture::SDLTexture(SDLTexture &&_texture) noexcept
        : _texture(_texture._texture), _size(_texture._size) {
  _texture._texture = nullptr;
}

sg::SDLTexture &sg::SDLTexture::operator=(SDLTexture &&other) noexcept {
  _texture = other._texture;
  other._texture = nullptr;
  return *this;
}

sg::SDLSurface::SDLSurface(SDLSurface &&o) noexcept : _surface(o._surface) {
  o._surface = nullptr;
}

sg::SDLSurface &sg::SDLSurface::operator=(SDLSurface &&o) noexcept {
  this->_surface = o._surface;
  o._surface = nullptr;
  return *this;
}

sg::SDLTexture::~SDLTexture() { SDL_DestroyTexture(_texture); }

sg::SDLWindow::SDLWindow(SDL_Window *const _window) : _window(_window) {}

sg::SDLWindow::~SDLWindow() { SDL_DestroyWindow(_window); }

sg::SDLRenderer sg::SDLWindow::create_renderer(IntVector const &v) {
  SDL_Renderer *const renderer{
          SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED)};
  if (renderer == nullptr)
    throw std::runtime_error{"couldn't initialize renderer: " +
                             sdl_error_string()};
  if (SDL_RenderSetLogicalSize(renderer, v.x(), v.y()) < 0)
    throw std::runtime_error{"couldn't initialize renderer (logical size): " +
                             sdl_error_string()};
  return SDLRenderer{renderer};
}

sg::SDLImageContext::SDLImageContext() {
  int const imgFlags{IMG_INIT_PNG};
  if (!(IMG_Init(imgFlags) & imgFlags)) // NOLINT(hicpp-signed-bitwise)
    throw std::runtime_error{"couldn't initialize sdl image context: " +
                             std::string{IMG_GetError()}};
}

sg::SDLImageContext::~SDLImageContext() { IMG_Quit(); }

sg::SDLContext::SDLContext() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    throw std::runtime_error{"couldn't initialize SDL: " +
                             sdl_error_string()};
}

std::vector<SDL_Event>
sg::SDLContext::wait_event(std::chrono::milliseconds const &s) {
  std::vector<SDL_Event> result;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    result.push_back(event);
  }
  if (result.empty())
    std::this_thread::sleep_for(s);
  return result;
}

sg::SDLContext::~SDLContext() { SDL_Quit(); }

sg::SDLWindow sg::SDLContext::create_window(IntVector const &v) {
  SDL_Window *const window{SDL_CreateWindow(
          "spacegame",
          SDL_WINDOWPOS_UNDEFINED,
          SDL_WINDOWPOS_UNDEFINED,
          v.x(),
          v.y(),
          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)};
  if (window == nullptr)
    throw std::runtime_error{"couldn't initialize window: " +
                             sdl_error_string()};
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
                             sdl_error_string()};
  return SDLTexture{texture};
}

void sg::SDLRenderer::clear() { SDL_RenderClear(_renderer); }

void sg::SDLRenderer::copy_whole(SDLTexture &t, IntRectangle const &r) {
  auto const dest_rect = to_sdl_rect(r);
  SDL_RenderCopy(_renderer, t.texture(), nullptr, &dest_rect);
}

void sg::SDLRenderer::copy(SDLTexture &t, IntRectangle const &from, IntRectangle const &to) {
  auto const from_rect = to_sdl_rect(from);
  auto const to_rect = to_sdl_rect(to);
  SDL_RenderCopy(_renderer, t.texture(), &from_rect, &to_rect);
}

void sg::SDLRenderer::present() { SDL_RenderPresent(_renderer); }

void sg::SDLRenderer::fill_rect(IntRectangle const &ext_rect, SDL_Color const &c) {
  const SDL_Rect &rect = to_sdl_rect(ext_rect);
  Uint8 r, g, b, a;
  if (SDL_GetRenderDrawColor(this->_renderer, &r, &g, &b, &a) != 0)
    throw std::runtime_error{"couldn't get render draw color " +
                             sdl_error_string()};
  if (SDL_SetRenderDrawColor(this->_renderer, c.r, c.g, c.b, c.a) != 0)
    throw std::runtime_error{"couldn't set render draw color " +
                             sdl_error_string()};
  if (SDL_RenderFillRect(this->_renderer, &rect) != 0)
    throw std::runtime_error{"couldn't fill rect " +
                             sdl_error_string()};
  if (SDL_SetRenderDrawColor(this->_renderer, r, g, b, a) != 0)
    throw std::runtime_error{"couldn't set render draw color " +
                             sdl_error_string()};

}

sg::SDLMixerContext::SDLMixerContext(SDLContext const &) : lib_inited_{false}, music_{nullptr} {
  if (Mix_Init(MIX_INIT_OPUS) != MIX_INIT_OPUS)
    throw std::runtime_error{"couldn't initialize SDL mixer: " + std::string{Mix_GetError()}};
  lib_inited_ = true;

  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1)
    throw std::runtime_error{"couldn't open audio in SDL mixer: " + std::string{Mix_GetError()}};
}

sg::SDLMixerContext::~SDLMixerContext() {
  if (lib_inited_)
    Mix_CloseAudio();
  Mix_Quit();
}

void sg::SDLMixerContext::play_music(std::filesystem::path const &p) {
  if (music_ != nullptr) {
    Mix_FreeMusic(music_);
    music_ = nullptr;
  }
  music_ = Mix_LoadMUS(p.c_str());
  if (music_ == nullptr)
    throw std::runtime_error{"couldn't load music " + p.string() + ": " + std::string{Mix_GetError()}};
  if (Mix_PlayMusic(music_, -1) == -1)
    throw std::runtime_error{"couldn't play music " + p.string() + ": " + std::string{Mix_GetError()}};
  Mix_VolumeMusic(MIX_MAX_VOLUME / 10);
}

sg::SDLMixerChunk::SDLMixerChunk(Mix_Chunk *_chunk) : chunk_(_chunk) {
}

sg::SDLMixerChunk::SDLMixerChunk(SDLMixerChunk &&other) noexcept : chunk_(other.chunk_) {
  other.chunk_ = nullptr;
}

sg::SDLMixerChunk &sg::SDLMixerChunk::operator=(SDLMixerChunk &&other) noexcept {
  this->chunk_ = other.chunk_;
  other.chunk_ = nullptr;
  return *this;
}

sg::SDLMixerChunk::~SDLMixerChunk() {
  Mix_FreeChunk(chunk_);
}

sg::SDLMixerChunk sg::SDLMixerContext::load_chunk(std::filesystem::path const &p) {
  Mix_Chunk *const chunk = Mix_LoadWAV(p.c_str());
  if (chunk == nullptr)
    throw std::runtime_error{"couldn't load " + p.string() + ": " + std::string{Mix_GetError()}};
  return sg::SDLMixerChunk(chunk);
}

void sg::SDLMixerContext::play_chunk(SDLMixerChunk &chunk) {
  Mix_PlayChannel(-1, chunk.chunk(), 0);
}

sg::SDLTTFContext::SDLTTFContext() {
  if (TTF_Init() == -1)
    throw std::runtime_error{"couldn't init TTF: " + std::string{TTF_GetError()}};
}

sg::SDLTTFContext::~SDLTTFContext() {
  TTF_Quit();
}

sg::SDLTTFFont sg::SDLTTFContext::open_font(std::filesystem::path const &p, unsigned const pt) {
  TTF_Font *const font{TTF_OpenFont(p.c_str(), static_cast<int>(pt))};
  if (font == nullptr)
    throw std::runtime_error{"couldn't load " + p.string() + ": " + std::string{TTF_GetError()}};
  return SDLTTFFont{font};
}


sg::SDLTTFFont::SDLTTFFont(TTF_Font *_font) : font_{_font} {
}

sg::SDLTTFFont::SDLTTFFont(SDLTTFFont &&other) noexcept {
  font_ = other.font_;
  other.font_ = nullptr;
}

sg::SDLTTFFont &sg::SDLTTFFont::operator=(SDLTTFFont &&other) noexcept {
  this->font_ = other.font_;
  other.font_ = nullptr;
  return *this;
}

sg::SDLSurface sg::SDLTTFFont::render_blended(std::string const &s, SDL_Color const &c) {
  return SDLSurface{TTF_RenderUTF8_Blended(font_, s.c_str(), c)};
}

sg::SDLTTFFont::~SDLTTFFont() {
  TTF_CloseFont(font_);
}
