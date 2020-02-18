#pragma once

#include "math.hpp"
#include "SDL.hpp"
#include "types.hpp"
#include "constants.hpp"
#include "RenderObject.hpp"
#include "Console.hpp"
#include "Animation.hpp"
#include <vector>
#include <list>

namespace sg {
enum class GameEvent {
  PlayerShot, AsteroidDestroyed
};
enum class EnemyType {
  AsteroidMedium
};
enum class ProjectileType {
  StandardLaser
};
struct EnemySpawn {
  EnemyType type;
  std::chrono::milliseconds spawn_after;
  DoubleVector spawn_position;
  Score score;
};

struct Asteroid {
  DoubleVector position;
  IntVector size;
  EnemyType type;
  Health health;
  Score score;

  Asteroid(const sg::DoubleVector &position, const sg::IntVector &size, EnemyType const &type, Health const health, Score const score)
          : position{position}, size{size}, type{type}, health{health}, score{score} {}
};

struct Particle {
  double lifetime;
  DoubleVector velocity;
  Animation animation;
};

struct Projectile {
  DoubleVector position;
  ProjectileType type;

  Projectile(const sg::DoubleVector &position, sg::ProjectileType type)
          : position(position), type(type) {}
};

using EventList = std::vector<sg::GameEvent>;

using SpawnList = std::list<sg::EnemySpawn>;

class GameState {
public:
  using ProjectileVector = std::vector<Projectile>;
  using AsteroidVector = std::vector<Asteroid>;

  GameState(RandomEngine &, Console &);

  [[nodiscard]] IntRectangle player_rect() const {
    return sg::IntRectangle::from_pos_and_size(
            rounding_cast<int>(player_position_), player_size);
  }

  void add_player_v(IntVector const &);

  EventList update(UpdateDiff const &);

  void player_shooting(bool b);

  RenderObjectList draw();

private:
  RandomEngine &random_engine_;
  Console &console_;
  Clock::time_point game_start_;
  SpawnList spawns_;
  DoubleVector player_position_;
  IntVector player_v_;
  bool player_shooting_;
  std::optional<TimePoint> last_shot_;
  ProjectileVector projectiles_;
  AsteroidVector asteroids_;
  Score score_;

  void process_spawns(
          Clock::time_point::duration const &);
};
}

