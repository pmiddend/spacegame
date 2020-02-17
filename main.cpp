#include "math.hpp"
#include "sdl.hpp"
#include "types.hpp"
#include "constants.hpp"
#include "game_state.hpp"
#include "starfield.hpp"
#include "sound_cache.hpp"
#include "texture_cache.hpp"
#include "atlas.hpp"
#include <SDL.h>
#include <chrono>
#include <iostream>
#include <optional>
#include <vector>

namespace {

std::filesystem::path const base_path{std::filesystem::path{"data"}};
std::filesystem::path const png_path{base_path / "PNG"};
std::filesystem::path const main_atlas_path{png_path / "main-atlas.png"};
std::string const ship_path{"playerShip1_blue.png"};
std::string const laser_path{"laserBlue01.png"};
std::filesystem::path const explosion_path{png_path / "explosion.png"};
sg::IntVector const explosion_tile_size{64, 64};
std::string const star_path{"star.png"};
std::filesystem::path const background_music{base_path / "music.opus"};
std::filesystem::path const pew_sound{base_path / "Bonus" / "sfx_laser1.wav"};
std::filesystem::path const font_path{base_path / "Bonus" / "kenvector_future_thin.ttf"};

void draw_gamestate(
        GameState const &gs,
        Atlas const &main_atlas,
        sg::SDLRenderer &renderer) {
  main_atlas.render_tile(renderer, ship_path, gs.player_rect());
  for (GameState::ProjectileVector::value_type const &p : gs.projectiles()) {
    main_atlas.render_tile(renderer, laser_path,
                           sg::IntRectangle::from_pos_and_size(sg::structure_cast<int>(p), projectile_size));
  }
}

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
  auto const pos = sg::IntVector{i % per_row, i / per_row} * tile_size_;
  renderer.copy(texture_, sg::IntRectangle::from_pos_and_size(pos, tile_size_), target);
}
} // namespace

int main() {
  sg::SDLContext context;
  sg::SDLMixerContext mixer_context{context};
  sg::SDLImageContext image_context;
  sg::SDLWindow window{context.create_window(game_size)};
  sg::SDLTTFContext ttfcontext;
  sg::SDLTTFFont main_font{ttfcontext.open_font(font_path, 15)};
  sg::SDLRenderer renderer{window.create_renderer(game_size)};
  sg::SDLSurface ship_surface{image_context.load_surface(
          std::filesystem::path{"data"} / "PNG" / "playerShip1_blue.png")};
  RandomEngine random_engine;
  GameState gs{random_engine};
  TextureCache texture_cache{image_context, renderer};
  Atlas main_atlas{texture_cache, main_atlas_path};
  Animation explosion_animation{texture_cache.get_texture(explosion_path), explosion_tile_size};
  SoundCache sound_cache{mixer_context};
  Starfield star_field{random_engine};
  std::cout << "game start\n";
  mixer_context.play_music(background_music);
  auto last_frame = Clock::now();
  auto const target_fps = std::chrono::milliseconds{16};
  while (true) {
    auto const this_frame{Clock::now()};
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
    for (GameEvent const &e : gs.update(second_delta)) {
      sound_cache.play_chunk(pew_sound);
    }
    star_field.update(second_delta);

    renderer.clear();
    star_field.draw(renderer, main_atlas, star_path);
    draw_gamestate(gs, main_atlas, renderer);
    explosion_animation.render_nth_tile(renderer, 4, sg::IntRectangle::from_pos_and_size(sg::IntVector{200, 200},
                                                                                         sg::IntVector{64, 64}));
    renderer.present();
  }
}
