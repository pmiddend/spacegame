#pragma once

#include <map>
#include <filesystem>
#include "SDL.hpp"

namespace sg {
class SoundCache {
private:
  using SoundMap = std::map<std::filesystem::path, sg::SDLMixerChunk>;

public:
  explicit SoundCache(sg::SDLMixerContext &_mixer_context)
          : mixer_context_{_mixer_context} {}


  void play_chunk(std::filesystem::path const &p) {
    SoundMap::iterator it{_sounds.find(p)};
    if (it != _sounds.end())
      mixer_context_.play_chunk(it->second);
    mixer_context_.play_chunk(_sounds.insert(SoundMap::value_type{p, mixer_context_.load_chunk(p)}).first->second);
  }

private:
  sg::SDLMixerContext &mixer_context_;
  SoundMap _sounds;
};
}
