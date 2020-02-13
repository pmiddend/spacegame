cmake_minimum_required(VERSION 3.16)

project(spacegame VERSION 1.0)

add_executable(spacegame main.cpp sdl.cpp game_state.cpp types.hpp constants.hpp texture_cache.hpp starfield.hpp starfield.cpp sound_cache.hpp)

set_target_properties(spacegame PROPERTIES CXX_STANDARD 17)
set_target_properties(spacegame PROPERTIES CXX_STANDARD_REQUIRED True)
target_compile_options(spacegame PRIVATE -Wall -Wextra)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
target_include_directories(spacegame PUBLIC ${SDL_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
target_link_libraries(spacegame ${SDL_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES})
install(TARGETS spacegame DESTINATION bin)
