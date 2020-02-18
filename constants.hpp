#pragma once

#include "SDL.hpp"
#include "TexturePath.hpp"
#include "FontDescriptor.hpp"
#include "types.hpp"

namespace sg {
IntVector const game_size{1024, 768};
IntRectangle const game_rect{sg::IntRectangle::from_pos_and_size(sg::IntVector{0, 0}, game_size)};
IntVector const player_size{50, 32};
IntVector const projectile_size{4, 26};
Health const asteroid_medium_health{50};
Health const projectile_damage{100};
DoubleVector const player_speed{200, 200};
DoubleVector const asteroid_medium_speed{0, 200};
double const projectile_speed{-300};
TexturePath const ship_path{"playerShip1_blue.png"};
TexturePath const laser_path{"laserBlue01.png"};
TexturePath const asteroid_medium_path{"meteorBrown_med1.png"};
TexturePath const star_path{"star.png"};
Color const console_background_color = {43, 43, 43, 128};
Color const console_font_color = {168, 176, 202, 255};
FontDescriptor const console_font{std::filesystem::path{"data"} / "Bonus" / "kenvector_future.ttf", 15};
}
