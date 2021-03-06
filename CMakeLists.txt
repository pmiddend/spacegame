cmake_minimum_required(VERSION 3.16)

project(spacegame VERSION 1.0)

add_executable(spacegame
        FontDescriptor.hpp
        FontCache.hpp
        main.cpp
        SDL.cpp
        GameState.cpp
        types.hpp
        constants.hpp
        TextureCache.hpp
        Starfield.hpp
        Starfield.cpp
        sound_cache.hpp
        Atlas.cpp
        Atlas.hpp
        RenderObject.hpp
        TexturePath.hpp
        GameState.hpp
        GameState.cpp util.hpp FontCache.cpp lru.hpp Console.cpp Console.hpp Animation.cpp Animation.hpp)

set_target_properties(spacegame PROPERTIES CXX_STANDARD 17)
set_target_properties(spacegame PROPERTIES CXX_STANDARD_REQUIRED True)
target_compile_options(spacegame PRIVATE -Wall -Wextra)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(SDL2 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
target_include_directories(spacegame PUBLIC
        ${SDL_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        )
target_link_libraries(spacegame
        ${SDL_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        nlohmann_json::nlohmann_json
        )
install(TARGETS spacegame DESTINATION bin)
