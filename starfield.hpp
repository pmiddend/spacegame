#pragma once

#include <vector>
#include "sdl.hpp"
#include "types.hpp"
#include "atlas.hpp"

class Starfield {
private:
    using LayerVector = std::vector<sg::DoubleVector>;
    using LayersVector = std::vector<LayerVector>;

public:
    sg::DoubleVector random_position();
    sg::DoubleVector random_top_position(unsigned const layer_index);
    explicit Starfield(RandomEngine &);
    void update(UpdateDiff const &);
    void draw(sg::SDLRenderer &, Atlas const &, std::string const &);
private:
    RandomEngine &random_engine_;
    std::uniform_real_distribution<double> distribution_x;
    std::uniform_real_distribution<double> distribution_y;
    LayersVector layers_;
};
