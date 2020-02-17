#pragma once

#include "math.hpp"
#include "sdl.hpp"
#include "types.hpp"
#include "constants.hpp"
#include <vector>
#include <list>

enum class GameEvent {
  PlayerShot
};

using EventList = std::vector<GameEvent>;

enum class EnemyType {
  Asteroid
};

struct EnemySpawn {
  EnemyType type;
  std::chrono::milliseconds spawn_after;
};

using SpawnList = std::list<EnemySpawn>;

struct Asteroid {
  sg::DoubleVector position;
  sg::IntVector size;
};

class GameState {
public:
  using ProjectileVector = std::vector<sg::DoubleVector>;
  using AsteroidVector = std::vector<Asteroid>;

  GameState(RandomEngine &);

  sg::IntRectangle player_rect() const {
    return sg::IntRectangle::from_pos_and_size(
            sg::structure_cast<int>(player_position_), player_size);
  }

  void add_player_v(sg::IntVector const &);

  EventList update(std::chrono::duration<double> const &);

  void player_shooting(bool b);

  ProjectileVector const &projectiles() const { return projectiles_; }

private:
  RandomEngine &random_engine_;
  Clock::time_point game_start_;
  SpawnList spawns_;
  sg::DoubleVector player_position_;
  sg::IntVector player_v_;
  bool player_shooting_;
  std::optional<TimePoint> last_shot_;
  ProjectileVector projectiles_;
  AsteroidVector asteroids_;

  void spawn_projectile();
};

