cmake_minimum_required(VERSION 3.24)
project(survival VERSION 0.0.1)

add_executable(
        ${PROJECT_NAME}
        src/Entity/Entity.cpp
        src/Font.cpp
        src/survival.cpp
        src/Texture.cpp
        src/LightMapTexture.cpp
        src/World.cpp
        src/utils.cpp
        src/Recipe.cpp
        src/Game.cpp
        src/SDLManager.cpp
        src/Color.cpp
        src/UIState.cpp
        src/Property.cpp
        src/Entity/PlayerEntity.cpp
        src/Entity/PlayerEntity.h
        src/Entity/NPCs/CatEntity.cpp
        src/Entity/NPCs/CatEntity.h
        src/Entity/NPCs/WolfEntity.cpp
        src/Entity/NPCs/WolfEntity.h
        src/Entity/NPCs/GlowbugEntity.cpp
        src/Entity/NPCs/GlowbugEntity.h
        src/Entity/NPCs/BunnyEntity.cpp
        src/Entity/NPCs/BunnyEntity.h
        src/Entity/BunnyHoleEntity.h
        src/Entity/Items/Food/EatableEntity.h
        src/Entity/Items/Food/CorpseEntity.h
        src/Entity/Items/Food/AppleEntity.h
        src/Entity/Items/Food/BerryEntity.h
        src/Entity/Sources/BushEntity.cpp
        src/Entity/Sources/BushEntity.h
        src/Entity/Items/Food/BananaEntity.h
        src/Entity/Sources/GrassEntity.cpp
        src/Entity/Sources/GrassEntity.h
        src/Entity/Items/Materials/GrassTuftEntity.h
        src/Entity/Items/Healing/BandageEntity.h
        src/Entity/Items/Materials/TwigEntity.h
        src/Entity/FireEntity.cpp
        src/Entity/FireEntity.h
        src/Entity/Items/TorchEntity.h
        src/Entity/ChestEntity.h
        src/Entity/Items/BagEntity.h
        src/Entity/WaterEntity.h
        src/Entity/Items/WaterskinEntity.h
        src/Entity/Building/BuildingWallEntity.cpp
        src/Entity/Building/BuildingWallEntity.h
        src/Entity/Building/DoorEntity.cpp
        src/Entity/Building/DoorEntity.h
        src/Entity/UI/StatusUIEntity.cpp
        src/Entity/UI/StatusUIEntity.h
        src/Behaviour/Behaviour.h
        src/Entity/EquipmentSlot.h
        src/Entity/EquipmentSlot.cpp
        src/Entity/EntityManager.cpp
        src/Entity/EntityManager.h
        src/Behaviour/AI/WanderBehaviour.cpp
        src/Behaviour/AI/WanderBehaviour.h
        src/Behaviour/AI/AttachmentBehaviour.cpp
        src/Behaviour/AI/AttachmentBehaviour.h
        src/Behaviour/AI/WanderAttachBehaviour.h
        src/Behaviour/AI/SeekHomeBehaviour.cpp
        src/Behaviour/AI/SeekHomeBehaviour.h
        src/Behaviour/AI/ChaseAndAttackBehaviour.cpp
        src/Behaviour/AI/ChaseAndAttackBehaviour.h
        src/Behaviour/AI/HostilityBehaviour.cpp
        src/Behaviour/AI/HostilityBehaviour.h
        src/Behaviour/Item/ApplyableBehaviour.h
        src/Behaviour/Item/HealingItemBehaviour.h
        src/Behaviour/Item/KeepStockedBehaviour.h
        src/Behaviour/InteractableBehaviour.h
        src/UI/MessageBoxRenderer.cpp
        src/UI/MessageBoxRenderer.h
        src/UI/Screens/Screen.cpp
        src/UI/Screens/Screen.h
        src/UI/NotificationMessageRenderer.cpp
        src/UI/NotificationMessageRenderer.h
        src/UI/Screens/NotificationMessageScreen.cpp
        src/UI/Screens/NotificationMessageScreen.h
        src/UI/Screens/InventoryScreen.cpp
        src/UI/Screens/InventoryScreen.h
        src/UI/Screens/LootingDialog.cpp
        src/UI/Screens/LootingDialog.h
        src/UI/Screens/InspectionDialog.cpp
        src/UI/Screens/InspectionDialog.h
        src/UI/Screens/CraftingScreen.cpp
        src/UI/Screens/CraftingScreen.h
        src/UI/Screens/EquipmentScreen.cpp
        src/UI/Screens/EquipmentScreen.h
        src/UI/Screens/HelpScreen.cpp
        src/UI/Screens/HelpScreen.h
        src/UI/Screens/DebugScreen.cpp
        src/UI/Screens/DebugScreen.h
        src/UI/Screens/Screens.h
        src/Behaviour/Item/HealingItemBehaviour.cpp
        src/Entity/Items/Food/EatableEntity.cpp
        src/Entity/Items/Healing/BandageEntity.cpp
        src/Entity/Items/Materials/GrassTuftEntity.cpp
        src/Entity/Items/Materials/TwigEntity.cpp
        src/Entity/Items/Food/AppleEntity.cpp
        src/Entity/Items/Food/BananaEntity.cpp
        src/Entity/Items/Food/BerryEntity.cpp
        src/Entity/Items/Food/CorpseEntity.cpp
        src/Entity/Items/BagEntity.cpp
        src/Entity/Items/TorchEntity.cpp
        src/Entity/Items/WaterskinEntity.cpp
        src/LightMapPoint.cpp
)

find_package(SDL3 CONFIG QUIET)

if (NOT SDL3_FOUND)
    message(STATUS "SDL3 not found. Building it from source.")
    include(FetchContent)
    FetchContent_Declare(
            SDL
            GIT_REPOSITORY https://github.com/libsdl-org/SDL
            GIT_TAG "release-3.4.2"
    )
    FetchContent_MakeAvailable(SDL)
else ()
    find_package(SDL3 REQUIRED)
endif ()

find_package(SDL3_image CONFIG QUIET)

if (NOT SDL3_image_FOUND)
    message(STATUS "SDL3_image not found. Building it from source.")
    include(FetchContent)
    FetchContent_Declare(
            SDL3_image
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
            GIT_TAG "release-3.4.2"
    )
    FetchContent_MakeAvailable(SDL3_image)
else ()
    find_package(SDL3_image REQUIRED)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

option(Emscripten "Build for Emscripten" OFF)

if (${Emscripten})
    message("Setting up for Emscsripten")
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "--preload-file ../resources/light.png@resources/light.png --preload-file ../resources/cursesV2_800x600.bmp@resources/cursesV2_800x600.bmp")

    add_custom_target(index.html ALL
            COMMAND cp "${CMAKE_SOURCE_DIR}/emscripten-template.html" "${CMAKE_CURRENT_BINARY_DIR}/index.html"
    )
else ()
    file(COPY resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()