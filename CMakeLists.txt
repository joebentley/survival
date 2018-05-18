cmake_minimum_required(VERSION 3.10)
project(survival)

file(COPY resources DESTINATION .)

add_executable(${PROJECT_NAME}
        src/behaviours.cpp
        src/behaviours.h
        src/UI.cpp
        src/UI.h
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
        src/world.h src/flags.h src/utils.h src/utils.cpp src/recipe.cpp src/recipe.h src/time.h)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_lambdas cxx_nullptr cxx_range_for cxx_strong_enums cxx_std_14)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -pedantic)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/sdl2-cmake-scripts")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE SDL2 SDL2_image)
target_link_libraries(${PROJECT_NAME} SDL2 SDL2_image)
