cmake_minimum_required(VERSION 3.10)
project(survival)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

file(COPY resources DESTINATION .)

add_executable(survival
        behaviours.cpp
        behaviours.h
        dialog.cpp
        dialog.h
        entities.cpp
        entities.h
        entity.cpp
        entity.h
        font.cpp
        font.h
        point.h
        survival.cpp
        texture.cpp
        texture.h
        world.cpp
        world.h flags.h)


INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES})