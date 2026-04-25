#include "FireEntity.h"

#include "../Property/Properties/LightEmittingProperty.h"
#include "../UI/MessageBoxRenderer.h"
#include "../UI/NotificationMessageRenderer.h"
#include "EntityManager.h"

FireEntity::FireEntity(std::string ID) : Entity(std::move(ID), "Fire", "") {
    mIsSolid = true;
    addProperty(std::make_unique<LightEmittingProperty>(this, 6));
    addBehaviour(std::make_unique<RekindleBehaviour>(*this));
}

void FireEntity::render(Font &font, Point currentWorldPos) {
    if (fireLevel < 0.1)
        mGraphic = "${black}$[grey]%";
    else if (rand() % 2 == 0)
        mGraphic = "${black}$[red]%";
    else
        mGraphic = "${black}$[orange]%";

    getProperty<LightEmittingProperty>()->setRadius(static_cast<int>(std::round(6 * fireLevel)));

    Entity::render(font, currentWorldPos);
}

void FireEntity::tick() { fireLevel -= 0.005f; }

bool FireEntity::RekindleBehaviour::handleInput(SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_J:
        if (choosingItemToUse) {
            const auto &player = EntityManager::getInstance().getEntityByID("Player");
            const auto &entities =
                player->filterInventoryForCraftingMaterials(std::vector<std::string>{"grass", "wood"});

            if ((size_t)choosingItemIndex == entities.size() - 1)
                choosingItemIndex = 0;
            else
                ++choosingItemIndex;
        }
        break;
    case SDLK_K:
        if (choosingItemToUse) {
            const auto &player = EntityManager::getInstance().getEntityByID("Player");
            const auto &entities =
                player->filterInventoryForCraftingMaterials(std::vector<std::string>{"grass", "wood"});

            if (choosingItemIndex == 0)
                choosingItemIndex = static_cast<int>(entities.size()) - 1;
            else
                ++choosingItemIndex;
        }
        break;
    case SDLK_RETURN:
        if (!choosingItemToUse) {
            choosingItemToUse = true;
            break;
        } else {
            const auto &player = EntityManager::getInstance().getEntityByID("Player");
            const auto &entities =
                player->filterInventoryForCraftingMaterials(std::vector<std::string>{"grass", "wood"});
            player->removeFromInventory(entities[choosingItemIndex]);
            dynamic_cast<FireEntity &>(mParent).fireLevel = 1;
            NotificationMessageRenderer::getInstance().queueMessage("$[red]Rekindled fire");
            choosingItemToUse = false;
            return false;
        }
    case SDLK_ESCAPE:
        if (choosingItemToUse) {
            choosingItemToUse = false;
            choosingItemIndex = 0;
            break;
        } else
            return false;
    }

    return true;
}

void FireEntity::RekindleBehaviour::render(Font & /*font*/) {
    MessageBoxRenderer::getInstance().queueMessageBoxCentered("$(right)Rekindle", 1);

    if (choosingItemToUse) {
        const auto &player = EntityManager::getInstance().getEntityByID("Player");
        const auto &entities = player->filterInventoryForCraftingMaterials(std::vector<std::string>{"grass", "wood"});

        if (entities.empty()) {
            choosingItemToUse = false;
            return;
        }

        std::vector<std::string> displayStrings;

        for (std::vector<std::string>::size_type i = 0; i < entities.size(); ++i) {
            const auto &entity = EntityManager::getInstance().getEntityByID(entities[i]);
            displayStrings.emplace_back((i == (size_t)choosingItemIndex ? "$(right)" : " ") + entity->mGraphic + " " +
                                        entity->mName);
        }

        MessageBoxRenderer::getInstance().queueMessageBoxCentered(displayStrings, 1);
    }
}
