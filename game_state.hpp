#pragma once

#include "math.hpp"
#include "sdl.hpp"
#include "types.hpp"
#include "constants.hpp"
#include <vector>

class GameState {
public:
    using ProjectileVector = std::vector<sg::DoubleVector>;

    GameState();

    sg::IntRectangle player_rect() const {
        return sg::IntRectangle::from_pos_and_size(
                sg::structure_cast<int>(player_position_), player_size);
    }

    void add_player_v(sg::IntVector const &);

    void update(std::chrono::duration<double> const &);

    void player_shooting(bool b);

    ProjectileVector const &projectiles() const { return projectiles_; }
private:
    sg::DoubleVector player_position_;
    sg::IntVector player_v_;
    bool player_shooting_;
    std::optional<TimePoint> last_shot_;
    ProjectileVector projectiles_;

    void spawn_projectile();
};

