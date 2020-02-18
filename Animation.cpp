#include "Animation.hpp"

#include <utility>

sg::Animation::Animation(AtlasDescriptor _descriptor, DoubleVector const &_position)
        : descriptor_{std::move(_descriptor)},
          position_{_position},
          current_{0},
          animation_{_descriptor.animation.value()} {
}

void sg::Animation::update(const AnimationDuration &d) {
  this->current_ += d;
}

sg::RenderObjectList sg::Animation::render() const {
  auto const ms_per_frame{this->animation_.duration.count() / this->animation_.tile_count};
  auto const frame_no{promoting_min(this->animation_.tile_count - 1, this->current_.count() / ms_per_frame)};
  return {Image(IntRectangle::from_pos_and_size(rounding_cast<int>(this->position_), this->animation_.tile_size),
                this->descriptor_,
                TexturePath{std::to_string(frame_no)})};
}

bool sg::Animation::done() const {
  return this->current_ > this->animation_.duration;
}

void sg::Animation::move(DoubleVector const &v) {
  this->position_ += v;
}
