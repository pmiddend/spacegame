#include <algorithm>
#include "game_state.hpp"

sg::GameState::GameState(RandomEngine &_random_engine)
        : random_engine_(_random_engine),
          player_position_{sg::structure_cast<double>(
                  game_size / 2 - player_size / 2)},
          player_v_{0, 0},
          spawns_{EnemySpawn{sg::EnemyType::Asteroid, std::chrono::milliseconds{2000}}},
          player_shooting_{false},
          game_start_{Clock::now()},
          asteroids_{} {}

void sg::GameState::add_player_v(sg::IntVector const &v) {
  player_v_ = sg::IntVector{player_v_.x() + v.x(), player_v_.y() + v.y()};
}

sg::EventList sg::GameState::update(UpdateDiff const &diff_secs) {
  auto const elapsed_time = Clock::now() - game_start_;
  for (SpawnList::iterator it{spawns_.begin()}; it != spawns_.end(); ++it) {
    if (it->spawn_after > elapsed_time)
      break;
    if (it->type == sg::EnemyType::Asteroid) {
      // FIXME
/*

      asteroids_.push_back(Asteroid{});
*/
    }
  }
  auto result = EventList{};
  double const secs = diff_secs.count();
  player_position_ =
          player_position_ +
          player_speed *
          (secs * sg::normalize(sg::structure_cast<double>(player_v_)));
  for (ProjectileVector::size_type i{0}; i < projectiles_.size(); ++i) {
    projectiles_[i] = projectiles_[i] + secs * sg::DoubleVector{0, projectile_speed};
  }
  projectiles_.erase(std::remove_if(projectiles_.begin(), projectiles_.end(), [](sg::DoubleVector const &v) {
    return !sg::rect_intersect(game_rect,
                               sg::IntRectangle::from_pos_and_size(sg::structure_cast<int>(v), projectile_size));
  }), projectiles_.end());
  auto const now = Clock::now();
  if (player_shooting_) {
    if (!last_shot_.has_value() || (now - last_shot_.value()) > std::chrono::milliseconds{500}) {
      result.push_back(sg::GameEvent::PlayerShot);
      projectiles_.push_back(player_position_ + sg::DoubleVector{player_size.x() / 2, 0});
      last_shot_ = now;
    }
  }
  return result;
}

void sg::GameState::player_shooting(bool b) {
  if (b == player_shooting_)
    return;
  player_shooting_ = b;
  if (!b)
    last_shot_ = std::nullopt;
}

void sg::GameState::spawn_projectile() {
  last_shot_ = Clock::now();

}

