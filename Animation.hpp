#pragma once

#include <utility>

#include "RenderObject.hpp"
#include "types.hpp"

namespace sg {
class Animation {
public:
  Animation(AtlasDescriptor, DoubleVector const &position);

  void update(AnimationDuration const &);
  [[nodiscard]] RenderObjectList render() const;
  [[nodiscard]] bool done() const;
  void move(DoubleVector const &);
private:
  AtlasDescriptor descriptor_;
  DoubleVector position_;
  AnimationDuration current_;
  AnimationDescriptor animation_;
};
}
