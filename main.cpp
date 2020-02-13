#include "math.hpp"
#include "sdl.hpp"
#include <SDL.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <vector>

namespace {
sg::IntVector const game_size{1024, 768};
sg::IntVector const player_size{99, 75};
sg::DoubleVector const player_speed{200, 200};
using UpdateDiff = std::chrono::duration<double>;
using RandomEngine = std::default_random_engine;

// class TexturePart {
// public:
//   TexturePart(
//       std::filesystem::path const &_file_path,
//       sg::IntRectangle const &_rectangle)
//       : file_path_{_file_path}, rectangle_(_rectangle) {}
//   std::filesystem::path const &file_path() const { return file_path_; }
//   sg::IntRectangle const &rectangle() const { return rectangle_; }

// private:
//   std::filesystem::path file_path_;
//   sg::IntRectangle rectangle_;
// };

std::filesystem::path const base_path{std::filesystem::path{"data"} / "PNG"};
std::filesystem::path const ship_path{base_path / "playerShip1_blue.png"};
std::filesystem::path const star_path{base_path / "star.png"};

class TextureCache {
private:
  using TextureMap = std::map<std::filesystem::path, sg::SDLTexture>;

public:
  TextureCache(sg::SDLImageContext &image_context, sg::SDLRenderer &renderer)
      : _image_context{image_context}, _renderer{renderer} {}

  sg::SDLTexture &get_texture(std::filesystem::path const &p) {
    TextureMap::iterator it{_textures.find(p)};
    if (it != _textures.end()) {
      return it->second;
    }
    auto surface = _image_context.load_surface(p);
    return _textures
        .insert(TextureMap::value_type{p, _renderer.create_texture(surface)})
        .first->second;
  }

private:
  sg::SDLImageContext &_image_context;
  sg::SDLRenderer &_renderer;
  TextureMap _textures;
};

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

class Starfield {
private:
  using LayerVector = std::vector<sg::DoubleVector>;
  using LayersVector = std::vector<LayerVector>;

  static unsigned star_count_per_layer(unsigned const layer_index) {
    return 85u * (layer_index + 1u) - 60u;
  }

  static sg::IntVector star_size_per_layer(unsigned const layer_index) {
    double const size{26 - 7.5 * (layer_index + 1)};
    return sg::IntVector{size, size};
  }

  static double star_speed_per_layer(unsigned const layer_index) {
    return 266 - 75 * (layer_index + 1);
  }

public:
  sg::DoubleVector random_position() {
    return sg::DoubleVector{distribution_x(random_engine_),
                            distribution_y(random_engine_)};
  }

  sg::DoubleVector random_top_position(unsigned const layer_index) {
    return sg::DoubleVector{distribution_x(random_engine_),
                            -star_size_per_layer(layer_index).y()};
  }

  explicit Starfield(RandomEngine &_random_engine)
      : random_engine_{_random_engine},
        distribution_x{0, static_cast<double>(game_size.x())},
        distribution_y{0, static_cast<double>(game_size.y())} {
    for (unsigned layer_index = 0; layer_index < 3; ++layer_index) {
      LayerVector new_layer;
      for (unsigned star_index = 0;
           star_index < star_count_per_layer(layer_index);
           ++star_index) {
        new_layer.push_back(random_position());
      }
      layers_.push_back(std::move(new_layer));
    }
  }

  void update(UpdateDiff const &d) {
    unsigned layer_index = 0;
    for (LayerVector &layer : layers_) {
      auto const star_speed{star_speed_per_layer(layer_index)};
      for (LayersVector::size_type i{0}; i < layer.size(); ++i) {
        layer[i] = layer[i] + d.count() * sg::DoubleVector{0.0, star_speed};
        if (layer[i].y() > game_size.y())
          layer[i] = random_top_position(layer_index - 1);
      }
      layer_index++;
    }
  }

  void draw(sg::SDLRenderer &renderer, sg::SDLTexture &texture) {
    unsigned layer_index{0};
    for (LayerVector const &layer : layers_) {
      auto const star_size{star_size_per_layer(layer_index)};
      for (sg::DoubleVector const &pos : layer) {
        sg::IntRectangle const star_rect{sg::IntRectangle::from_pos_and_size(
            sg::structure_cast<int>(pos), star_size)};
        renderer.copy(texture, star_rect);
      }
      layer_index++;
    }
  }

private:
  RandomEngine &random_engine_;
  std::uniform_real_distribution<double> distribution_x;
  std::uniform_real_distribution<double> distribution_y;
  LayersVector layers_;
};

GameState::GameState()
    : player_position{sg::structure_cast<double>(
          game_size / 2 - player_size / 2)},
      player_v{0, 0} {}

void GameState::add_player_v(sg::IntVector const &v) {
  player_v = sg::IntVector{player_v.x() + v.x(), player_v.y() + v.y()};
}

void GameState::update(UpdateDiff const &diff_secs) {
  double const secs = diff_secs.count();
  player_position =
      player_position +
      player_speed *
          (secs * sg::normalize(sg::structure_cast<double>(player_v)));
}

void draw(
    GameState const &gs,
    sg::SDLRenderer &renderer,
    TextureCache &texture_cache) {
  renderer.copy(texture_cache.get_texture(ship_path), gs.player_rect());
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
  TextureCache texture_cache{image_context, renderer};
  RandomEngine random_engine;
  Starfield star_field{random_engine};
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

    auto const second_delta{
        std::chrono::duration_cast<std::chrono::duration<double>>(time_delta)};
    gs.update(second_delta);
    star_field.update(second_delta);

    // star_field.draw(renderer, texture_cache.get_texture(star_path));
    renderer.clear();
    star_field.draw(renderer, texture_cache.get_texture(star_path));
    draw(gs, renderer, texture_cache);
    renderer.present();
  }
}
