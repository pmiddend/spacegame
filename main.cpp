#include <SDL.h>
#include <cstdlib>
#include <iostream>

namespace {
class SDLContext {
public:
  SDLContext();
};

SDLContext::SDLContext() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw new std::runtime_error{"couldn't initialize SDL: " +
                                 std::string(SDL_GetError())};
  }
}
} // namespace

int main() {
  SDLContext context;

  SDL_Window *const window = SDL_CreateWindow(
      "spacegame",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      640,
      480,
      SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    std::cerr << "couldn't initialize window: " << SDL_GetError() << "\n";
    return EXIT_FAILURE;
  }
}
