#pragma once

#include "RenderObject.hpp"
#include "types.hpp"

namespace sg {
struct AnimationDescriptor {
  SDLTexture *texture;
  IntVector tile_size;
  unsigned fps;

  AnimationDescriptor(SDLTexture &texture, const IntVector &tile_size, unsigned int fps)
          : texture{&texture}, tile_size{tile_size}, fps{fps} {
  }
};

class Animation {
public:
  explicit Animation(AnimationDescriptor const &);

  void update(UpdateDiff const &);
  [[nodiscard]] RenderObjectList render() const;
private:
  AnimationDescriptor descriptor_;
  UpdateDiff current_;
};
}
