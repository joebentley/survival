#include "ChoosingPositionInspectionDialogState.h"

#include "../../../Entity/EntityManager.h"
#include "../../../Entity/PlayerEntity.h"
#include "../../../World.h"
#include "../../Screens/InspectionDialog.h"
#include "ViewingDescriptionInspectionDialogState.h"

void ChoosingPositionInspectionDialogState::onEntry(InspectionDialog & /*screen*/) {}

std::unique_ptr<InspectionDialogState> ChoosingPositionInspectionDialogState::handleInput(InspectionDialog &screen,
                                                                                          SDL_KeyboardEvent &e) {
    mChosenPoint = screen.getChosenPoint();

    Point posOffset;

    switch (e.key) {
    case SDLK_ESCAPE:
        screen.disable();
        return nullptr;
    case SDLK_H:
        posOffset = Point(-1, 0);
        break;
    case SDLK_J:
        posOffset = Point(0, 1);
        break;
    case SDLK_K:
        posOffset = Point(0, -1);
        break;
    case SDLK_L:
        posOffset = Point(1, 0);
        break;
    case SDLK_Y:
        posOffset = Point(-1, -1);
        break;
    case SDLK_U:
        posOffset = Point(1, -1);
        break;
    case SDLK_B:
        posOffset = Point(-1, 1);
        break;
    case SDLK_N:
        posOffset = Point(1, 1);
        break;
    case SDLK_EQUALS:
        if (screen.isSelectingFromMultipleOptions()) {
            const auto &currentEntities = EntityManager::getInstance().getEntitiesAtPosFaster(mChosenPoint);
            if ((size_t)mChosenIndex == currentEntities.size() - 1)
                mChosenIndex = 0;
            else
                mChosenIndex++;
            screen.setChosenIndex(mChosenIndex);
        }
        break;
    case SDLK_MINUS:
        if (screen.isSelectingFromMultipleOptions()) {
            const auto &currentEntities = EntityManager::getInstance().getEntitiesAtPosFaster(mChosenPoint);
            if (mChosenIndex == 0)
                mChosenIndex = (int)(currentEntities.size() - 1);
            else
                mChosenIndex--;
            screen.setChosenIndex(mChosenIndex);
        }
        break;
    case SDLK_RETURN:
        if (screen.isThereAnEntity())
            return std::make_unique<ViewingDescriptionInspectionDialogState>();
        break;
    }

    if (e.mod & SDL_KMOD_SHIFT) // NOLINT(hicpp-signed-bitwise)
        posOffset *= 5;

    mChosenPoint = clipToScreenEdge(screen, mChosenPoint + posOffset);

    screen.setChosenPoint(mChosenPoint);

    return nullptr;
}

void ChoosingPositionInspectionDialogState::onExit(InspectionDialog & /*screen*/) {}

inline Point ChoosingPositionInspectionDialogState::clipToScreenEdge(InspectionDialog &screen, const Point &p) const {
    auto worldPos = screen.getPlayer().getWorldPos();
    auto point = p - Point(World::SCREEN_WIDTH, World::SCREEN_HEIGHT) * worldPos;
    Point returnPoint(p);
    if (point.mX < 0)
        returnPoint.mX = worldPos.mX * World::SCREEN_WIDTH;
    else if (point.mX > World::SCREEN_WIDTH - 1)
        returnPoint.mX = (worldPos.mX + 1) * World::SCREEN_WIDTH - 1;
    if (point.mY < 0)
        returnPoint.mY = worldPos.mY * World::SCREEN_HEIGHT;
    else if (point.mY > World::SCREEN_HEIGHT - 1)
        returnPoint.mY = (worldPos.mY + 1) * World::SCREEN_HEIGHT - 1;
    return returnPoint;
}
