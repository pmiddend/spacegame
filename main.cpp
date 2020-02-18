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

std::filesystem::path const background_music{sg::base_path / "music.opus"};
std::filesystem::path const pew_sound{sg::base_path / "Bonus" / "sfx_laser1.wav"};
std::filesystem::path const explosion_short_sound{sg::base_path / "explosion-short.wav"};
std::filesystem::path const font_path{sg::base_path / "Bonus" / "kenvector_future_thin.ttf"};

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

struct RenderObjectVisitor {
  sg::SDLRenderer &renderer;
  sg::AtlasCache &atlas_cache;
  sg::FontCache &font_cache;

  RenderObjectVisitor(sg::SDLRenderer &renderer, sg::AtlasCache &atlas_cache, sg::FontCache &font_cache)
          : renderer{renderer}, atlas_cache{atlas_cache}, font_cache{font_cache} {}

  void operator()(sg::Image const &image) const {
    atlas_cache.get(image.atlas).render_tile(renderer, image.texture, image.rectangle);
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
  sg::AtlasCache atlas_cache{texture_cache};
  sg::FontCache font_cache{ttfcontext, renderer};
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
    auto const wait_time{std::chrono::duration_cast<std::chrono::milliseconds>(target_fps - time_delta)};
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

    auto const second_delta{std::chrono::duration_cast<std::chrono::duration<double>>(time_delta)};
    for (sg::GameEvent const &ge : gs.update(second_delta)) {
      switch (ge) {
        case sg::GameEvent::PlayerShot:
          sound_cache.play_chunk(pew_sound);
          break;
        case sg::GameEvent::AsteroidDestroyed:
          sound_cache.play_chunk(explosion_short_sound);
          break;
      }
    }
    star_field.update(second_delta);

    renderer.clear();
    for (auto const &rob : star_field.draw())
      std::visit(RenderObjectVisitor(renderer, atlas_cache, font_cache), rob);
    for (const auto &rob : gs.draw())
      std::visit(RenderObjectVisitor(renderer, atlas_cache, font_cache), rob);
    for (const auto &rob : console.draw())
      std::visit(RenderObjectVisitor(renderer, atlas_cache, font_cache), rob);
    renderer.present();
  }
}
