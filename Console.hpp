#pragma once

#include <vector>
#include <string>
#include "RenderObject.hpp"

namespace sg {
class Console {
public:
  using LineVector = std::vector<std::string>;

  Console();

  [[nodiscard]] sg::RenderObjectList draw() const;

  void toggle();

  void add_line(std::string const &, bool with_date);

private:
  LineVector lines_;
  bool toggled_;
};
}
