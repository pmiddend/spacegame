cmake_minimum_required(VERSION 3.16)

project(spacegame VERSION 1.0)

add_executable(spacegame main.cpp)

set_target_properties(spacegame PROPERTIES CXX_STANDARD 17)
set_target_properties(spacegame PROPERTIES CXX_STANDARD_REQUIRED True)
target_compile_options(spacegame PRIVATE -Wall -Wextra)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(SDL2 REQUIRED)
MESSAGE(${SDL_INCLUDE_DIR})
target_include_directories(spacegame PUBLIC ${SDL_INCLUDE_DIR})
target_link_libraries(spacegame ${SDL_LIBRARIES})
install(TARGETS spacegame DESTINATION bin)
