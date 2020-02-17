#include <algorithm>
#include <iostream>
#include "game_state.hpp"

namespace {
sg::DoubleVector enemy_type_to_speed(sg::EnemyType const &t) {
  switch (t) {
    case sg::EnemyType::AsteroidMedium:
      return asteroid_medium_speed;
  }
  throw std::runtime_error("cannot determine speed for enemy type");
}

sg::Health enemy_type_to_health(sg::EnemyType const &t) {
  switch (t) {
    case sg::EnemyType::AsteroidMedium:
      return 100;
  }
  throw std::runtime_error("cannot determine health for enemy type");
}

template<typename Container, typename F>
void erase_if(Container &container, F const &f) {
  container.erase(std::remove_if(container.begin(), container.end(), f), container.end());
}
}

sg::GameState::GameState(RandomEngine &_random_engine)
        : random_engine_(_random_engine),
          game_start_{Clock::now()},
          spawns_{EnemySpawn{sg::EnemyType::AsteroidMedium, std::chrono::milliseconds{2000}, DoubleVector{120, -43}}},
          player_position_{sg::structure_cast<double>(
                  game_size / 2 - player_size / 2)},
          player_v_{0, 0},
          player_shooting_{false} {}

void sg::GameState::add_player_v(sg::IntVector const &v) {
  player_v_ = sg::IntVector{player_v_.x() + v.x(), player_v_.y() + v.y()};
}

sg::EventList sg::GameState::update(UpdateDiff const &diff_secs) {
  auto const elapsed_time = Clock::now() - game_start_;
  process_spawns(elapsed_time);
  auto result = EventList{};
  double const secs{diff_secs.count()};
  player_position_ +=
          player_speed *
          (secs * sg::normalize(sg::structure_cast<double>(player_v_)));
  for (ProjectileVector::size_type i{0}; i < projectiles_.size(); ++i)
    projectiles_[i] = projectiles_[i] + secs * sg::DoubleVector{0, projectile_speed};

  erase_if(projectiles_, [](sg::DoubleVector const &v) {
    return !sg::rect_intersect(game_rect,
                               sg::IntRectangle::from_pos_and_size(sg::structure_cast<int>(v), projectile_size));
  });

  for (Asteroid &asteroid : asteroids_)
    asteroid.position += enemy_type_to_speed(asteroid.type);

  erase_if(asteroids_, [](sg::Asteroid const &v) {
    return !sg::rect_intersect(game_rect,
                               sg::IntRectangle::from_pos_and_size(sg::structure_cast<int>(v.position), v.size));
  });

  auto const now = Clock::now();
  if (player_shooting_) {
    if (!last_shot_.has_value() || (now - last_shot_.value()) > std::chrono::milliseconds{500}) {
      result.push_back(sg::GameEvent::PlayerShot);
      projectiles_.push_back(player_position_ + sg::DoubleVector{static_cast<double>(player_size.x()) / 2.0, 0});
      last_shot_ = now;
    }
  }
  return result;
}

void sg::GameState::process_spawns(
        Clock::time_point::duration const &elapsed_time) {
  for (sg::SpawnList::iterator it{spawns_.begin()}; it != spawns_.end(); ++it) {
    if (it->spawn_after > elapsed_time)
      break;
    if (it->type == sg::EnemyType::AsteroidMedium) {
      auto size = sg::IntVector{43, 43};
      std::cout << "spawning asteroid\n";
      asteroids_.push_back(sg::Asteroid{it->spawn_position, size, it->type, enemy_type_to_health(it->type)});
    }
    it = spawns_.erase(it);
  }
}

void sg::GameState::player_shooting(bool b) {
  if (b == player_shooting_)
    return;
  player_shooting_ = b;
  if (!b)
    last_shot_ = std::nullopt;
}

sg::RenderObjectList sg::GameState::draw() {
  sg::RenderObjectList result{sg::Image(player_rect(), ship_path)};
  for (sg::GameState::ProjectileVector::value_type const &p : projectiles_)
    result.push_back(Image{sg::IntRectangle::from_pos_and_size(sg::structure_cast<int>(p), projectile_size),
                           laser_path});
  return result;
}
