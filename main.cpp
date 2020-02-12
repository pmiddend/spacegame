#include "math.hpp"
#include "sdl.hpp"
#include <SDL.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>

namespace {
sg::IntVector const game_size{1024, 768};
sg::IntVector const player_size{99, 75};
sg::DoubleVector const player_speed{200, 200};

class GameState {
public:
  GameState();

  sg::IntRectangle player_rect() const {
    return sg::IntRectangle::from_pos_and_size(
        sg::structure_cast<int>(player_position), player_size);
  }

  void add_player_v(sg::IntVector const &);

  void update(std::chrono::duration<double> const &);

private:
  sg::DoubleVector player_position;
  sg::IntVector player_v;
};

GameState::GameState()
    : player_position{sg::structure_cast<double>(
          game_size / 2 - player_size / 2)},
      player_v{0, 0} {}

void GameState::add_player_v(sg::IntVector const &v) {
  player_v = sg::IntVector{player_v.x() + v.x(), player_v.y() + v.y()};
}

void GameState::update(std::chrono::duration<double> const &diff_secs) {
  double const secs = diff_secs.count();
  std::cout << "normalized: "
            << (player_speed *
                (secs * sg::normalize(sg::structure_cast<double>(player_v))))
            << "\n";
  player_position =
      player_position +
      player_speed *
          (secs * sg::normalize(sg::structure_cast<double>(player_v)));
}

void draw(
    GameState const &gs,
    sg::SDLRenderer &renderer,
    sg::SDLTexture &ship_texture) {
  renderer.clear();
  renderer.copy(ship_texture, gs.player_rect());
  renderer.present();
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
} // namespace

int main() {
  sg::SDLContext context;
  sg::SDLImageContext image_context;
  sg::SDLWindow window{context.create_window(game_size)};
  sg::SDLRenderer renderer{window.create_renderer(game_size)};
  sg::SDLSurface ship_surface{image_context.load_surface(
      std::filesystem::path{"data"} / "PNG" / "playerShip1_blue.png")};
  sg::SDLTexture ship_texture{renderer.create_texture(ship_surface)};
  bool done{false};
  GameState gs;
  std::cout << "game start\n";
  auto last_frame = std::chrono::system_clock::now();
  auto const target_fps = std::chrono::milliseconds{16};
  while (!done) {
    auto const this_frame{std::chrono::system_clock::now()};
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
        auto const direction = key_to_direction(e.value().key.keysym.sym);
        if (direction.has_value())
          gs.add_player_v(direction.value());
      } else if (e.value().type == SDL_KEYUP && e.value().key.repeat == 0) {
        auto const direction = key_to_direction(e.value().key.keysym.sym);
        if (direction.has_value())
          gs.add_player_v(-direction.value());
      }
    }

    gs.update(
        std::chrono::duration_cast<std::chrono::duration<double>>(time_delta));

    draw(gs, renderer, ship_texture);
  }
}
