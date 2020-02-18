#include "Console.hpp"
#include "constants.hpp"

namespace {
std::string format_hms(sg::Clock::time_point const &t) {
  std::time_t const now{std::chrono::system_clock::to_time_t(t)};
  char s[32];
  std::strftime(&s[0], 32, "%H:%M:%S", std::localtime(&now));
  return std::string{s};
}
}

sg::Console::Console() : lines_{}, toggled_{false} {
}

void sg::Console::toggle() {
  this->toggled_ = !this->toggled_;
}

void sg::Console::add_line(const std::string &l, bool const date) {
  this->lines_.push_back(date ? (format_hms(Clock::now())+": ")+l : l);
}

sg::RenderObjectList sg::Console::draw() const {
  if (!this->toggled_)
    return {};

  sg::RenderObjectList result;
  result.push_back(sg::Solid{sg::IntRectangle::from_size_at_origin(sg::IntVector{game_size.x(), game_size.y() / 2}),
                             console_background_color});
  std::size_t const line_count{game_size.y() / 2 / console_font.size + 1};
  std::size_t i{0};
  for (LineVector::const_reverse_iterator it{this->lines_.crbegin()}; i < std::min(line_count, this->lines_.size()); ++i, ++it)
    result.push_back(sg::Text{console_font, *it, sg::IntVector{0, static_cast<int>(game_size.y() / 2 - (i+1) * console_font.size)}, console_font_color});
  return result;
}
