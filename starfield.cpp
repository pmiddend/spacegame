#include "starfield.hpp"
#include "constants.hpp"
#include "atlas.hpp"

namespace {
static unsigned star_count_per_layer(unsigned const layer_index) {
    return 85u * (layer_index + 1u) - 60u;
}

static sg::IntVector star_size_per_layer(unsigned const layer_index) {
    double const size{26 - 7.5 * (layer_index + 1)};
    return sg::IntVector{static_cast<int>( size ), static_cast<int>( size )};
}

static double star_speed_per_layer(unsigned const layer_index) {
    return 266 - 75 * (layer_index + 1);
}

}

sg::DoubleVector sg::Starfield::random_position() {
    return sg::DoubleVector{distribution_x(random_engine_),
                            distribution_y(random_engine_)};
}

sg::DoubleVector sg::Starfield::random_top_position(unsigned const layer_index) {
    return sg::DoubleVector{distribution_x(random_engine_),
                            -star_size_per_layer(layer_index).y()};
}

sg::Starfield::Starfield(RandomEngine &_random_engine)
        : random_engine_{_random_engine},
          distribution_x{0, static_cast<double>(game_size.x())},
          distribution_y{0, static_cast<double>(game_size.y())} {
    for (unsigned layer_index = 0; layer_index < 3; ++layer_index) {
        LayerVector new_layer;
        for (unsigned star_index = 0;
             star_index < star_count_per_layer(layer_index);
             ++star_index) {
            new_layer.push_back(random_position());
        }
        layers_.push_back(std::move(new_layer));
    }
}

void sg::Starfield::update(UpdateDiff const &d) {
    unsigned layer_index = 0;
    for (LayerVector &layer : layers_) {
        auto const star_speed{star_speed_per_layer(layer_index)};
        for (LayersVector::size_type i{0}; i < layer.size(); ++i) {
            layer[i] = layer[i] + d.count() * sg::DoubleVector{0.0, star_speed};
            if (layer[i].y() > game_size.y())
                layer[i] = random_top_position(layer_index - 1);
        }
        layer_index++;
    }
}

void sg::Starfield::draw(sg::SDLRenderer &renderer, sg::Atlas const &atlas, std::string const &atlas_image) {
    LayersVector::size_type layer_index{layers_.size() - 1};
    for (LayersVector::const_reverse_iterator layer_it{layers_.crbegin()}; layer_it != layers_.crend(); ++layer_it) {
        auto const star_size{star_size_per_layer(layer_index)};
        for (sg::DoubleVector const &pos : (*layer_it)) {
            sg::IntRectangle const star_rect{sg::IntRectangle::from_pos_and_size(
                    sg::structure_cast<int>(pos), star_size)};
            atlas.render_tile(renderer, atlas_image, star_rect);
        }
        layer_index--;
    }
}

