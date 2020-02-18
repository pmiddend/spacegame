#pragma once

#include <chrono>
#include <random>
#include <SDL.h>

namespace sg {
using UpdateDiff = std::chrono::duration<double>;
using RandomEngine = std::default_random_engine;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Color = SDL_Color;
using Health = int;
using Score = int;
}
