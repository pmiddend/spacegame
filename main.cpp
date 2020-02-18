#include "math.hpp"
#include "SDL.hpp"
#include "types.hpp"
#include "constants.hpp"
#include "GameState.hpp"
#include "Starfield.hpp"
#include "sound_cache.hpp"
#include "TextureCache.hpp"
#include "Atlas.hpp"
#include "FontCache.hpp"
#include "Console.hpp"
#include <SDL.h>
#include <chrono>
#include <iostream>
#include <optional>

namespace {

std::filesystem::path const base_path{std::filesystem::path{"data"}};
std::filesystem::path const png_path{base_path / "PNG"};
std::filesystem::path const main_atlas_path{png_path / "main-atlas.png"};
std::filesystem::path const explosion_path{png_path / "explosion.png"};
sg::IntVector const explosion_tile_size{64, 64};
std::filesystem::path const background_music{base_path / "music.opus"};
std::filesystem::path const pew_sound{base_path / "Bonus" / "sfx_laser1.wav"};
std::filesystem::path const font_path{base_path / "Bonus" / "kenvector_future_thin.ttf"};

std::optional<sg::IntVector> key_to_direction(SDL_Keycode const &k) {
  if (k == SDLK_a)
    return sg::IntVector{-1, 0};
  if (k == SDLK_d)
    return sg::IntVector{1, 0};
  if (k == SDLK_w)
    return sg::IntVector{0, -1};
  if (k == SDLK_s)
    return sg::IntVector{0, 1};
  return std::nullopt;
}

class Animation {
public:
  Animation(sg::SDLTexture &, sg::IntVector const &);

  sg::SDLTexture &texture() { return texture_; }

  void render_nth_tile(sg::SDLRenderer &renderer, std::size_t, sg::IntRectangle const &) const;

private:
  sg::SDLTexture &texture_;
  sg::IntVector tile_size_;
};

Animation::Animation(sg::SDLTexture &_texture, sg::IntVector const &_tile_size) : texture_(_texture),
                                                                                  tile_size_(_tile_size) {}

void Animation::render_nth_tile(sg::SDLRenderer &renderer, std::size_t const i, sg::IntRectangle const &target) const {
  int const per_row{texture_.size().x() / tile_size_.x()};
  auto const pos = sg::IntVector{static_cast<int>(i % per_row), static_cast<int>(i / per_row)} * tile_size_;
  renderer.copy(texture_, sg::IntRectangle::from_pos_and_size(pos, tile_size_), target);
}

struct RenderObjectVisitor {
  sg::SDLRenderer &renderer;
  sg::Atlas &main_atlas;
  sg::FontCache &font_cache;

  RenderObjectVisitor(sg::SDLRenderer &renderer, sg::Atlas &mainAtlas, sg::FontCache &font_cache)
          : renderer{renderer}, main_atlas{mainAtlas}, font_cache{font_cache} {}

  void operator()(sg::Image const &image) const {
    main_atlas.render_tile(renderer, image.texture, image.rectangle);
  }

  void operator()(sg::Solid const &s) const {
    renderer.fill_rect(s.rectangle, s.color);
  }

  void operator()(sg::Text const &t) const {
    font_cache.copy_text(t.font, t.text, t.color, t.position);
  }
};


} // namespace

int main() {
  sg::Console console{};
  sg::SDLContext context;
  sg::SDLMixerContext mixer_context{context};
  sg::SDLImageContext image_context;
  sg::SDLWindow window{context.create_window(sg::game_size)};
  sg::SDLTTFContext ttfcontext;
  sg::SDLTTFFont main_font{ttfcontext.open_font(font_path, 15)};
  sg::SDLRenderer renderer{window.create_renderer(sg::game_size)};
  sg::RandomEngine random_engine;
  sg::GameState gs{random_engine, console};
  TextureCache texture_cache{image_context, renderer};
  sg::FontCache font_cache{ttfcontext, renderer};
  sg::Atlas main_atlas{texture_cache, main_atlas_path};
  //Animation explosion_animation{texture_cache.get_texture(explosion_path), explosion_tile_size};
  sg::SoundCache sound_cache{mixer_context};
  sg::Starfield star_field{random_engine};
  std::cout << "game start\n";
  mixer_context.play_music(background_music);
  auto last_frame = sg::Clock::now();
  auto const target_fps = std::chrono::milliseconds{16};
  while (true) {
    auto const this_frame{sg::Clock::now()};
    auto const time_delta{this_frame - last_frame};
    auto const wait_time{std::chrono::duration_cast<std::chrono::milliseconds>(
            target_fps - time_delta)};
    last_frame = this_frame;
    std::optional<SDL_Event> const e{context.wait_event(wait_time)};
    if (e.has_value()) {
      if (e.value().type == SDL_QUIT)
        break;

      if (e.value().type == SDL_KEYDOWN && e.value().key.repeat == 0) {
        if (e.value().key.keysym.sym == SDLK_ESCAPE)
          break;
        if (e.value().key.keysym.sym == SDLK_BACKQUOTE)
          console.toggle();
        if (e.value().key.keysym.sym == SDLK_SPACE)
          gs.player_shooting(true);
        auto const direction = key_to_direction(e.value().key.keysym.sym);
        if (direction.has_value())
          gs.add_player_v(direction.value());
      } else if (e.value().type == SDL_KEYUP && e.value().key.repeat == 0) {
        if (e.value().key.keysym.sym == SDLK_SPACE)
          gs.player_shooting(false);
        auto const direction = key_to_direction(e.value().key.keysym.sym);
        if (direction.has_value())
          gs.add_player_v(-direction.value());
      }
    }

    auto const second_delta{
            std::chrono::duration_cast<std::chrono::duration<double>>(time_delta)};
    for (sg::GameEvent const &e : gs.update(second_delta))
      sound_cache.play_chunk(pew_sound);
    star_field.update(second_delta);

    renderer.clear();
    for (auto const &rob : star_field.draw())
      std::visit(RenderObjectVisitor(renderer, main_atlas, font_cache), rob);
    for (const auto &rob : gs.draw())
      std::visit(RenderObjectVisitor(renderer, main_atlas, font_cache), rob);
    for (const auto &rob : console.draw())
      std::visit(RenderObjectVisitor(renderer, main_atlas, font_cache), rob);
    renderer.present();
  }
}
