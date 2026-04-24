#include "InspectionDialog.h"

#include "../../Entity/EntityManager.h"
#include "../../utils.h"
#include "../MessageBoxRenderer.h"

void InspectionDialog::handleInput(SDL_KeyboardEvent &e) {
    auto newState = mState->handleInput(*this, e);
    if (newState != nullptr) {
        mState->onExit(*this);
        mState = std::move(newState);
        mState->onEntry(*this);
    }

    mShouldRenderWorld = !mViewingDescription;
}

void InspectionDialog::render(Font &font) {
    const auto &entitiesAtPointBefore = EntityManager::getInstance().getEntitiesAtPosFaster(mChosenPoint);
    std::vector<Entity *> entitiesAtPoint;

    std::copy_if(entitiesAtPointBefore.cbegin(), entitiesAtPointBefore.cend(), std::back_inserter(entitiesAtPoint),
                 [](auto &a) { return !a->mIsInAnInventory; });

    if (mViewingDescription) {
        drawDescriptionScreen(font, *entitiesAtPoint[mChosenIndex]);
        return;
    }

    const auto &chosenPointScreen = World::worldToScreen(mChosenPoint);
    font.drawText("${black}$[yellow]X", chosenPointScreen);

    int xPosWindow = chosenPointScreen.mX >= World::SCREEN_WIDTH / 2 ? 2 : World::SCREEN_WIDTH / 2 + 1;

    if (entitiesAtPoint.size() > 1) {
        mSelectingFromMultipleOptions = true;
        std::vector<std::string> lines;

        lines.emplace_back(" You see");
        std::transform(entitiesAtPoint.cbegin(), entitiesAtPoint.cend(), std::back_inserter(lines),
                       [](auto &a) -> std::string { return " " + a->mGraphic + " " + a->mName; });

        lines.emplace_back("");
        lines.emplace_back(" (-)-$(up) (=)-$(down) return-desc");
        showMessageBox(font, lines, 1, xPosWindow - 1, 2);
        font.draw("right", xPosWindow + 2 - 1, 2 + 3 + mChosenIndex);
        mThereIsAnEntity = true;
    } else {
        mSelectingFromMultipleOptions = false;
        mChosenIndex = 0;
    }

    if (entitiesAtPoint.size() == 1) {
        const auto &entity = *entitiesAtPoint[0];

        std::vector<std::string> lines;
        lines.emplace_back(entity.mGraphic + " " + entity.mName);

        if (!entity.mShortDesc.empty()) {
            lines.emplace_back("");
            auto descLines = wordWrap(entity.mShortDesc, World::SCREEN_WIDTH / 2 - 5);
            std::copy(descLines.begin(), descLines.end(), std::back_inserter(lines));
        }

        if (!entity.mLongDesc.empty()) {
            lines.emplace_back("");
            auto descLines = wordWrap(entity.mLongDesc, World::SCREEN_WIDTH / 2 - 5);
            std::copy(descLines.begin(), descLines.end(), std::back_inserter(lines));
        }

        const int cappedNumLines = World::SCREEN_HEIGHT - 20;
        std::vector<std::string> linesCapped = lines;
        if (lines.size() > cappedNumLines) {
            linesCapped = std::vector<std::string>(lines.begin(), lines.begin() + cappedNumLines);
            *(linesCapped.end() - 1) += "...";
        }

        MessageBoxRenderer::getInstance().queueMessageBox(lines, 1, xPosWindow, 2);

        mThereIsAnEntity = true;
    }

    if (entitiesAtPoint.empty())
        mThereIsAnEntity = false;
}

PlayerEntity &InspectionDialog::getPlayer() { return mPlayer; }

const Point &InspectionDialog::getChosenPoint() const { return mChosenPoint; }

void InspectionDialog::setChosenPoint(const Point &chosenPoint) { mChosenPoint = chosenPoint; }

bool InspectionDialog::isThereAnEntity() const { return mThereIsAnEntity; }

void InspectionDialog::setViewingDescription(bool viewingDescription) { mViewingDescription = viewingDescription; }

bool InspectionDialog::isSelectingFromMultipleOptions() const { return mSelectingFromMultipleOptions; }

void InspectionDialog::setChosenIndex(int chosenIndex) { mChosenIndex = chosenIndex; }
