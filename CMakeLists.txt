cmake_minimum_required(VERSION 3.10)
project(survival)

file(COPY resources DESTINATION .)

if(MSVC)
	file(GLOB DLLS libs/*)
	file(COPY ${DLLS} DESTINATION .)
endif(MSVC)

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
		src/World.h src/flags.h src/utils.h src/utils.cpp src/Recipe.cpp src/Recipe.h src/Time.h src/Game.h src/Game.cpp src/SDLManager.cpp src/SDLManager.h src/FontColor.cpp src/FontColor.h src/EntityBuilder.h src/UIState.cpp src/UIState.h src/Property.cpp src/Property.h src/Properties.h)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_lambdas cxx_nullptr cxx_range_for cxx_strong_enums cxx_std_14)

if(NOT MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -pedantic)
endif(NOT MSVC)


if(MSVC)
	# set to your SDL2 install directory if using windows
	if(x86)
		set(SDL2_PATH "C:\\Users\\Joe\\Source\\SDL2\\lib\\x86")
		set(SDL2_IMAGE_PATH "C:\\Users\\Joe\\Source\\SDL2_image\\lib\\x86")
	else(x86)
		set(SDL2_PATH "C:\\Users\\Joe\\Source\\SDL2\\lib\\x64")
		set(SDL2_IMAGE_PATH "C:\\Users\\Joe\\Source\\SDL2_image\\lib\\x64")
	endif(x86)
	set(SDL2_INCLUDE_DIR "C:\\Users\\Joe\\Source\\SDL2\\include")
	set(SDL2_IMAGE_INCLUDE_DIR "C:\\Users\\Joe\\Source\\SDL2_image\\include")
endif(MSVC)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/sdl2-cmake-scripts")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

if(MSVC)
	target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES})
else(MSVC)
	target_include_directories(${PROJECT_NAME} PRIVATE SDL2 SDL2_image)
	target_link_libraries(${PROJECT_NAME} SDL2 SDL2_image)
endif(MSVC)

find_package(Boost REQUIRED)

if(MSVC)
	target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})
else(MSVC)
	target_include_directories(${PROJECT_NAME} PRIVATE Boost)
endif(MSVC)

if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()