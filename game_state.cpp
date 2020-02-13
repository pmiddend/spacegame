#include <algorithm>
#include <iostream>
#include "game_state.hpp"

GameState::GameState()
        : player_position_{sg::structure_cast<double>(
        game_size / 2 - player_size / 2)},
          player_v_{0, 0}, player_shooting_{false} {}

void GameState::add_player_v(sg::IntVector const &v) {
    player_v_ = sg::IntVector{player_v_.x() + v.x(), player_v_.y() + v.y()};
}

void GameState::update(UpdateDiff const &diff_secs) {
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
            projectiles_.push_back(player_position_ + sg::DoubleVector{player_size.x()/2,0});
            last_shot_ = now;
        }
    }
}

void GameState::player_shooting(bool b) {
    if (b == player_shooting_)
        return;
    player_shooting_ = b;
    spawn_projectile();
}

void GameState::spawn_projectile() {
    last_shot_ = Clock::now();

}

