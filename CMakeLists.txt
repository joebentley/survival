cmake_minimum_required(VERSION 3.24)
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

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

find_package(SDL3 CONFIG QUIET)

if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found. Building it from source.")
    include(FetchContent)
    FetchContent_Declare(
            SDL
            GIT_REPOSITORY https://github.com/libsdl-org/SDL
            GIT_TAG        "release-3.4.2"
    )
    FetchContent_MakeAvailable(SDL)
else()
    find_package(SDL3 REQUIRED)
endif()

find_package(SDL3_image CONFIG QUIET)

if(NOT SDL3_image_FOUND)
    message(STATUS "SDL3_image not found. Building it from source.")
    include(FetchContent)
    FetchContent_Declare(
            SDL3_image
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
            GIT_TAG        "release-3.4.2"
    )
    FetchContent_MakeAvailable(SDL3_image)
else()
    find_package(SDL3_image REQUIRED)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image)

# Create the output dir and copy the resources folder into it
set(output_dir ${CMAKE_CURRENT_BINARY_DIR}/survival-${CMAKE_PROJECT_VERSION})
file(MAKE_DIRECTORY ${output_dir})
file(COPY resources DESTINATION ${output_dir})
file(COPY unquarantine.sh DESTINATION ${output_dir})

# After build, copy binary into output dir
add_custom_command(TARGET survival POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:survival> ${output_dir}
)
