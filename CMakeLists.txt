cmake_minimum_required(VERSION 3.10)
project(survival VERSION 0.0.1)

add_executable(
	${PROJECT_NAME}
	src/Behaviours.cpp
	src/UI.cpp
	src/Entities.cpp
	src/Entity.cpp
	src/Font.cpp
	src/survival.cpp
	src/Texture.cpp
	src/World.cpp
	src/utils.cpp
	src/Recipe.cpp
	src/Game.cpp
	src/SDLManager.cpp
	src/FontColor.cpp
	src/UIState.cpp
	src/Property.cpp
)

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

# Create the output dir and copy the resources folder into it
set(output_dir ${CMAKE_CURRENT_BINARY_DIR}/survival-${CMAKE_PROJECT_VERSION})
file(MAKE_DIRECTORY ${output_dir})
file(COPY resources DESTINATION ${output_dir})
file(COPY unquarantine.sh DESTINATION ${output_dir})

# After build, copy binary into output dir
add_custom_command(TARGET survival POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:survival> ${output_dir}
)
