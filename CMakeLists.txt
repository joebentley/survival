cmake_minimum_required(VERSION 3.10)
project(survival)

add_executable(${PROJECT_NAME}
        src/Behaviours.cpp
        src/Behaviours.h
        src/UI.cpp
        src/UI.h
		src/Entities.cpp
		src/Entities.h
		src/Entity.cpp
		src/Entity.h
		src/Font.cpp
		src/Font.h
		src/Point.h
        src/survival.cpp
		src/Texture.cpp
		src/Texture.h
		src/World.cpp
		src/World.h src/flags.h src/utils.h src/utils.cpp src/Recipe.cpp src/Recipe.h src/Time.h src/Game.h src/Game.cpp src/SDLManager.cpp src/SDLManager.h src/FontColor.cpp src/FontColor.h src/EntityBuilder.h src/UIState.cpp src/UIState.h src/Property.cpp src/Property.h src/Properties.h src/settings.h)

file(COPY resources DESTINATION .)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_lambdas cxx_nullptr cxx_range_for cxx_strong_enums cxx_std_17)

if(NOT MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -pedantic)
endif(NOT MSVC)

# Create an option to switch between a system sdl library and a vendored sdl library
option(${PROJECT_NAME}_VENDORED "Use vendored libraries" OFF)

if(${PROJECT_NAME}_VENDORED)
    add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

	find_package(SDL2_image REQUIRED CONFIG REQUIRED COMPONENTS SDL2_image)

    # 1. Look for a SDL2 package, 2. Look for the SDL2maincomponent and 3. DO NOT fail when SDL2main is not available
    find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
endif()

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
	target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_image::SDL2_image)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_image::SDL2_image)
