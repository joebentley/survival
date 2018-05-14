cmake_minimum_required(VERSION 3.10)
project(survival)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")

include_directories(.)

file(COPY resources DESTINATION .)

add_executable(survival
        src/behaviours.cpp
        src/behaviours.h
        src/dialog.cpp
        src/dialog.h
        src/entities.cpp
        src/entities.h
        src/entity.cpp
        src/entity.h
        src/font.cpp
        src/font.h
        src/point.h
        src/survival.cpp
        src/texture.cpp
        src/texture.h
        src/world.cpp
        src/world.h src/flags.h src/utils.h src/utils.cpp)


INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES})