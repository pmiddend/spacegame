#pragma once

#include "sdl.hpp"
#include "texture_path.hpp"

namespace {
sg::IntVector const game_size{1024, 768};
sg::IntRectangle const game_rect{sg::IntRectangle::from_pos_and_size(sg::IntVector{0,0}, game_size)};
sg::IntVector const player_size{50, 32};
sg::IntVector const projectile_size{4, 26};
sg::DoubleVector const player_speed{200, 200};
double const projectile_speed{-300};
sg::texture_path const ship_path{"playerShip1_blue.png"};
sg::texture_path const laser_path{"laserBlue01.png"};
sg::texture_path const star_path{"star.png"};
}
