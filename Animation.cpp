#include "Animation.hpp"

sg::Animation::Animation(const sg::AnimationDescriptor &_descriptor) : descriptor_{_descriptor}, current_{0} {

}

void sg::Animation::update(const sg::UpdateDiff &d) {
  this->current_ += d;
}

sg::RenderObjectList sg::Animation::render() const {
  return sg::RenderObjectList();
}
