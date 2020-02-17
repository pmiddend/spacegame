#pragma once

#include "math.hpp"
#include "sdl.hpp"
#include "types.hpp"
#include "constants.hpp"
#include "render_object.hpp"
#include <vector>
#include <list>

namespace sg {
enum class GameEvent {
  PlayerShot
};
enum class EnemyType {
  Asteroid
};
struct EnemySpawn {
  EnemyType type;
  std::chrono::milliseconds spawn_after;
};
struct Asteroid {
  DoubleVector position;
  IntVector size;
};

using EventList = std::vector<sg::GameEvent>;

using SpawnList = std::list<sg::EnemySpawn>;

class GameState {
public:
  using ProjectileVector = std::vector<DoubleVector>;
  using AsteroidVector = std::vector<Asteroid>;

  explicit GameState(RandomEngine &);

  [[nodiscard]] IntRectangle player_rect() const {
    return sg::IntRectangle::from_pos_and_size(
            structure_cast<int>(player_position_), player_size);
  }

  void add_player_v(IntVector const &);

  EventList update(std::chrono::duration<double> const &);

  void player_shooting(bool b);

  [[nodiscard]] ProjectileVector const &projectiles() const { return projectiles_; }

  RenderObjectList draw();

private:
  RandomEngine &random_engine_;
  Clock::time_point game_start_;
  SpawnList spawns_;
  DoubleVector player_position_;
  IntVector player_v_;
  bool player_shooting_;
  std::optional<TimePoint> last_shot_;
  ProjectileVector projectiles_;
  AsteroidVector asteroids_;

  void spawn_projectile();
};
}

