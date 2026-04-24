#include "PlayerEntity.h"

#include "../Behaviour/InteractableBehaviour.h"
#include "../UI/MessageBoxRenderer.h"
#include "../UI/NotificationMessageRenderer.h"
#include "../UI/Screens/Screen.h"
#include "../UI/Screens/Screens.h"
#include "../flags.h"
#include "UI/StatusUIEntity.h"

bool PlayerEntity::attack(const Point &attackPos) {
    auto entitiesInSquare = EntityManager::getInstance().getEntitiesAtPosFaster(attackPos);

    if (entitiesInSquare.empty()) {
        return false;
    }

    auto enemy = entitiesInSquare[0];
    // roll and damage the enemy
    int damage = rollDamage();
    enemy->mHp -= damage;

    // send hit notification message
    NotificationMessageRenderer::getInstance().queueMessage(
        "$(dwarf) hit " + enemy->mGraphic + " " + enemy->mName + "$[white] with $[red]$(heart)$[white]" +
        std::to_string(mHitTimes) + "d" + std::to_string(mHitAmount) + " for " + std::to_string(damage));

    // TODO: Add AV
    // TODO: Add avoidance
    // force enemy to start attacking player
    if (enemy->getBehaviourByID("WanderBehaviour") != nullptr)
        enemy->getBehaviourByID("WanderBehaviour")->disable();
    if (enemy->getBehaviourByID("AttachmentBehaviour") != nullptr)
        enemy->getBehaviourByID("AttachmentBehaviour")->disable();
    if (enemy->getBehaviourByID("WanderAttachBehaviour") != nullptr)
        enemy->getBehaviourByID("WanderAttachBehaviour")->disable();
    if (enemy->getBehaviourByID("ChaseAndAttackBehaviour") != nullptr)
        enemy->getBehaviourByID("ChaseAndAttackBehaviour")->enable();

    // set the player's attack target to the enemy
    auto &ui = dynamic_cast<StatusUIEntity &>(*EntityManager::getInstance().getEntityByID("StatusUI"));
    ui.setAttackTarget(enemy->mID);

    // if the enemy died, delete the enemy and stop attacking
    if (enemy->mHp <= 0) {
        ui.clearAttackTarget();
        EntityManager::getInstance().queueForDeletion(enemy->mID);
        attacking = false;
        NotificationMessageRenderer::getInstance().queueMessage(enemy->mGraphic + " " + enemy->mName +
                                                                "$[white] was ${black}$[red]destroyed!");
        return false;
    }

    attacking = true;
    return true;
}

void PlayerEntity::tick() {
    hunger -= hungerRate;

    if (hunger < 0.3)
        mHp -= hungerDamageRate;

    Entity::tick();
}

void PlayerEntity::handleInput(SDL_KeyboardEvent &e, bool &quit,
                               std::unordered_map<ScreenType, std::unique_ptr<Screen>> &screens) {
    auto key = e.key;
    auto mod = e.mod;

    bool didAction = false;

    if (showingTooMuchWeightMessage) {
        if (key == SDLK_ESCAPE || key == SDLK_RETURN)
            showingTooMuchWeightMessage = false;
        return;
    }

    // Keep interacting with the entity, bypass other interactions
    if (interactingWithEntity) {
        auto entity = EntityManager::getInstance().getEntityByID(mEntityInteractingWith);
        auto b = entity->getBehaviourByID("InteractableBehaviour");
        if (!dynamic_cast<InteractableBehaviour &>(*b).handleInput(e)) {
            mEntityInteractingWith.clear();
            interactingWithEntity = false;
        }
        return;
    }

    if (mHp > 0) {
        // Handle interaction
        if (key == SDLK_SPACE) {
            auto entitiesSurrounding = EntityManager::getInstance().getEntitiesSurroundingFaster(mPos);

            // Just use the first interactable entity found
            for (auto &entity : entitiesSurrounding) {
                auto b = entity->getBehaviourByID("InteractableBehaviour");

                if (b != nullptr) {
                    interactingWithEntity = true;
                    mEntityInteractingWith = entity->mID;

                    // perform an initial interaction
                    if (!dynamic_cast<InteractableBehaviour &>(*b).handleInput(e)) {
                        mEntityInteractingWith.clear();
                        interactingWithEntity = false;
                    }
                }
            }
        }

        // Handle looting
        if (key == SDLK_G) {
            auto entitiesAtPos = EntityManager::getInstance().getEntitiesAtPosFaster(mPos);

            // TODO: need to handle multiple items on same square properly
            //            std::vector<std::shared_ptr<Entity>> waterEntities;

            std::vector<Entity *> entitiesWithInventories;
            std::copy_if(entitiesAtPos.begin(), entitiesAtPos.end(), std::back_inserter(entitiesWithInventories),
                         [](auto &a) { return a->mID != "Player" && !a->isInventoryEmpty(); });

            // TODO: chests are broken right now (no way to open)!

            if (!entitiesWithInventories.empty()) {
                // TODO: what if multiple chests are present?
                auto entity = entitiesWithInventories[0];
                if (entity->mSkipLootingDialog) {
                    // Loot just the first item
                    if (addToInventory(entity->getInventoryItem(0)->mID)) {
                        entity->removeFromInventory(0);
                        didAction = true;
                        goto postLooting;
                    } else {
                        showingTooMuchWeightMessage = true;
                        return;
                    }
                } else {
                    auto &lootingDialog = dynamic_cast<LootingDialog &>(*screens[ScreenType::LOOTING]);
                    lootingDialog.showItemsToLoot(entity->getInventory(), entity);
                    return;
                }
            }

            std::vector<Entity *> pickuppableEntities;
            std::copy_if(entitiesAtPos.begin(), entitiesAtPos.end(), std::back_inserter(pickuppableEntities),
                         [this](auto &a) {
                             auto b = a->hasProperty("Pickuppable");
                             // Don't pick up if it isn't pickuppable, or if it is already in the player's inventory
                             return (b && !isInInventory(a->mID));
                         });

            if (pickuppableEntities.empty())
                return;
            else if (pickuppableEntities.size() == 1) {
                if (addToInventory((*pickuppableEntities.begin())->mID)) {
                    didAction = true;
                    goto postLooting;
                } else {
                    showingTooMuchWeightMessage = true;
                    return;
                }
            } else {
                auto &lootingDialog = dynamic_cast<LootingDialog &>(*screens[ScreenType::LOOTING]);
                lootingDialog.showItemsToLoot(pickuppableEntities);
                return;
            }
        }

    postLooting:

        if (mHp > 0 && (key == SDLK_H || key == SDLK_J || key == SDLK_K || key == SDLK_L || key == SDLK_Y ||
                        key == SDLK_U || key == SDLK_B || key == SDLK_N)) // Only ever attack if moving
        {
            Point posOffset;
            switch (key) {
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
            default:
                posOffset = Point(0, 0);
                break;
            }

            Point newPos = getPos() + posOffset;

            std::vector<Entity *> entitiesInSpace = EntityManager::getInstance().getEntitiesAtPosFaster(newPos);

            if (!entitiesInSpace.empty()) {
                // TODO: what if more than one enemy in space?
                Entity *entity = entitiesInSpace[0];

                // Check whether or not to attack
                if (entity->canBeAttacked() && ((entity->getBehaviourByID("HostilityBehaviour") != nullptr &&
                                                 entity->getBehaviourByID("HostilityBehaviour")->isEnabled()) ||
                                                (mod & SDL_KMOD_SHIFT) // force attack // NOLINT(hicpp-signed-bitwise)
                                                || attacking           // already attacking
                                                )) {
                    attack(newPos);
                    didAction = true;
                }
            }

            // Try to move into the new space if we didn't attack
            if (!didAction && !moveTo(newPos))
                return; // Don't tick if didn't move

            didAction = true;
        }

        if (key == SDLK_I) {
            screens[ScreenType::INVENTORY]->enable();
        }

        if (key == SDLK_PERIOD) {
            EntityManager::getInstance().broadcast(SIGNAL_FORCE_WAIT);
            didAction = true;
        }

        if (key == SDLK_SEMICOLON) {
            auto &b = dynamic_cast<InspectionDialog &>(*screens[ScreenType::INSPECTION]);
            b.enableAtPoint(mPos);
        }

        if (key == SDLK_C)
            screens[ScreenType::CRAFTING]->enable();

        if (key == SDLK_E)
            screens[ScreenType::EQUIPMENT]->enable();

        if (key == SDLK_M)
            screens[ScreenType::NOTIFICATION]->enable();

        if (mod & SDL_KMOD_SHIFT && key == SDLK_SLASH)
            screens[ScreenType::HELP]->enable();

        if (mod & SDL_KMOD_SHIFT && key == SDLK_D)
            screens[ScreenType::DEBUG]->enable();
    }

    if (didAction)
        EntityManager::getInstance().tick();

    if (mHp <= 0 && key == SDLK_RETURN)
        quit = true;
}

void PlayerEntity::render(Font &font, Point currentWorldPos) {
    Entity::render(font, currentWorldPos);

    if (showingTooMuchWeightMessage) {
        const std::string &displayString = "You cannot carry that much!";
        MessageBoxRenderer::getInstance().queueMessageBoxCentered(displayString, 1);
    }

    if (interactingWithEntity) {
        auto entityInteractingWith = EntityManager::getInstance().getEntityByID(mEntityInteractingWith);
        dynamic_cast<InteractableBehaviour &>(*entityInteractingWith->getBehaviourByID("InteractableBehaviour"))
            .render(font);
    }
}

bool PlayerEntity::addToInventory(const std::string &ID) {
    if (Entity::addToInventory(ID)) {
        auto entity = EntityManager::getInstance().getEntityByID(ID);
        NotificationMessageRenderer::getInstance().queueMessage("You got a " + entity->mGraphic + " " + entity->mName +
                                                                "${transparent}$[white]!");
        return true;
    }
    return false;
}

void PlayerEntity::addHunger(float hungerRestoration) { hunger = std::min(hunger + hungerRestoration, 1.0f); }
