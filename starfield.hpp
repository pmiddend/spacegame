#pragma once

#include <vector>
#include "sdl.hpp"
#include "types.hpp"
#include "atlas.hpp"
#include "render_object.hpp"

namespace sg {
class Starfield {
private:
    using LayerVector = std::vector<DoubleVector>;
    using LayersVector = std::vector<LayerVector>;

public:
    DoubleVector random_position();
    DoubleVector random_top_position(unsigned layer_index);
    explicit Starfield(RandomEngine &);
    void update(UpdateDiff const &);
    RenderObjectList draw();
private:
    RandomEngine &random_engine_;
    std::uniform_real_distribution<double> distribution_x;
    std::uniform_real_distribution<double> distribution_y;
    LayersVector layers_;
};
}
