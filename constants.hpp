#pragma once

#include "sdl.hpp"

sg::IntVector const game_size{1024, 768};
sg::IntRectangle const game_rect{sg::IntRectangle::from_pos_and_size(sg::IntVector{0,0}, game_size)};
sg::IntVector const player_size{50, 32};
sg::IntVector const projectile_size{4, 26};
sg::DoubleVector const player_speed{200, 200};
double const projectile_speed{-300};
