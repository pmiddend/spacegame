#include <algorithm>
#include <iostream>
#include "GameState.hpp"

namespace {
sg::DoubleVector enemy_type_to_speed(sg::EnemyType const &t) {
  switch (t) {
    case sg::EnemyType::AsteroidMedium:
      return sg::asteroid_medium_speed;
  }
  throw std::runtime_error("cannot determine speed for enemy type");
}

sg::Health enemy_type_to_health(sg::EnemyType const &t) {
  switch (t) {
    case sg::EnemyType::AsteroidMedium:
      return sg::asteroid_medium_health;
  }
  throw std::runtime_error("cannot determine health for enemy type");
}

template<typename Container, typename F>
void erase_if(Container &container, F const &f) {
  container.erase(std::remove_if(container.begin(), container.end(), f), container.end());
}

template<typename T>
sg::Rectangle<T> projectile_rect(sg::Projectile const &v) {
  return sg::Rectangle<T>::from_pos_and_size(sg::structure_cast<T>(v.position),
                                             sg::structure_cast<T>(sg::projectile_size));
}

template<typename T>
sg::Rectangle<T> asteroid_rect(sg::Asteroid const &v) {
  return sg::embiggen(sg::Rectangle<T>::from_pos_and_size(sg::structure_cast<T>(v.position),
                                                          sg::structure_cast<T>(v.size)), 0.75);
}
}

sg::GameState::GameState(RandomEngine &_random_engine, Console &_console)
        : random_engine_{_random_engine},
          console_{_console},
          game_start_{Clock::now()},
          spawns_{EnemySpawn{sg::EnemyType::AsteroidMedium,
                             std::chrono::milliseconds{2000},
                             DoubleVector{120, -43},
                             1}},
          player_position_{sg::structure_cast<double>(game_size / 2 - player_size / 2)},
          player_v_{0, 0},
          player_shooting_{false},
          score_{0} {}

void sg::GameState::add_player_v(sg::IntVector const &v) {
  player_v_ = sg::IntVector{player_v_.x() + v.x(), player_v_.y() + v.y()};
}

sg::EventList sg::GameState::update(IntUpdateDiff const &diff_secs) {
  auto const elapsed_time = Clock::now() - game_start_;
  process_spawns(elapsed_time);
  auto result = EventList{};
  double const secs{std::chrono::duration_cast<DoubleUpdateDiff>(diff_secs).count()};

  // Move player
  player_position_ += player_speed * (secs * sg::normalize(sg::structure_cast<double>(player_v_)));

  // Move projectiles
  for (ProjectileVector::size_type i{0}; i < projectiles_.size(); ++i)
    projectiles_[i].position += secs * sg::DoubleVector{0, projectile_speed};

  auto const double_game_rect{structure_cast<double>(game_rect)};
  auto const bigger_game_rect(embiggen<double>(double_game_rect, 2));

  // Remove projectiles that are out of screen
  erase_if(projectiles_, [&bigger_game_rect](Projectile const &v) {
    return !sg::rect_intersect(bigger_game_rect, projectile_rect<double>(v));
  });

  // Move asteroids
  for (Asteroid &asteroid : asteroids_)
    asteroid.position += secs * enemy_type_to_speed(asteroid.type);

  // Remove asteroids that are out of screen
  erase_if(asteroids_, [&bigger_game_rect, this](sg::Asteroid const &v) {
    bool const result{!sg::rect_intersect(bigger_game_rect, asteroid_rect<double>(v))};
    if (result)
      console_.add_line("removing asteroid", true);
    return result;
  });

  // Handle asteroid projectile collisions
  for (ProjectileVector::iterator pit{this->projectiles_.begin()}; pit != this->projectiles_.end();) {
    auto const prect{projectile_rect<double>(*pit)};
    bool found{false};
    for (AsteroidVector::iterator ait{this->asteroids_.begin()}; ait != this->asteroids_.end(); ++ait) {
      if (rect_intersect(prect, asteroid_rect<double>(*ait))) {
        found = true;
        ait->health -= projectile_damage;
        if (ait->health <= 0) {
          score_ += ait->score;
          result.push_back(GameEvent::AsteroidDestroyed);
          particles_.push_back(Particle{DoubleVector{0, 0}, Animation{explosion_animation, ait->position}});
          this->asteroids_.erase(ait);
        }
        break;
      }
    }
    if (found)
      pit = this->projectiles_.erase(pit);
    else
      ++pit;
  }

  // Add projectiles
  auto const now = Clock::now();
  if (player_shooting_) {
    if (!last_shot_.has_value() || (now - last_shot_.value()) > std::chrono::milliseconds{500}) {
      result.push_back(sg::GameEvent::PlayerShot);
      projectiles_.push_back(Projectile{
              player_position_ + sg::DoubleVector{static_cast<double>(player_size.x()) / 2.0, 0},
              ProjectileType::StandardLaser});
      last_shot_ = now;
    }
  }

  // Handle particles
  // Update/move particles
  for (Particle &p : particles_) {
    p.animation.move(p.velocity * secs);
    p.animation.update(diff_secs);

  }
  // Remove stale particles
  erase_if(particles_, [](sg::Particle const &v) { return v.animation.done(); });

  return result;
}

void sg::GameState::process_spawns(Clock::time_point::duration const &elapsed_time) {
  for (sg::SpawnList::iterator it{spawns_.begin()}; it != spawns_.end(); ++it) {
    if (it->spawn_after > elapsed_time)
      break;
    if (it->type == sg::EnemyType::AsteroidMedium) {
      auto size = sg::IntVector{42, 42};
      console_.add_line("spawning asteroid", true);
      asteroids_.push_back(sg::Asteroid{it->spawn_position, size, it->type, enemy_type_to_health(it->type), it->score});
    }
    it = spawns_.erase(it);
  }
}

void sg::GameState::player_shooting(bool const b) {
  if (b == player_shooting_)
    return;
  player_shooting_ = b;
  if (!b)
    last_shot_ = std::nullopt;
}

sg::RenderObjectList sg::GameState::draw() {
  sg::RenderObjectList result{sg::Image(player_rect(), main_atlas_path, ship_path)};
  for (sg::GameState::ProjectileVector::value_type const &p : projectiles_)
    result.push_back(Image{sg::IntRectangle::from_pos_and_size(sg::rounding_cast<int>(p.position), projectile_size),
                           main_atlas_path,
                           laser_path});
  for (sg::GameState::AsteroidVector::value_type const &p : asteroids_)
    result.push_back(Image{sg::IntRectangle::from_pos_and_size(sg::rounding_cast<int>(p.position), p.size),
                           main_atlas_path,
                           asteroid_medium_path});
  for (sg::GameState::ParticleVector ::value_type const &p : particles_)
    append(result, p.animation.render());
  result.push_back(sg::Text{score_font, "Score: " + std::to_string(score_), IntVector{0, 0}, score_color});
  return result;
}
