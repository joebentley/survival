#include "entities.h"
#include "world.h"

#include <algorithm>

bool PlayerEntity::attack(const Point &attackPos) {
    auto entitiesInSquare = EntityManager::getInstance().getEntitiesAtPosFaster(attackPos);

    if (entitiesInSquare.empty()) {
        return false;
    }

    auto enemy = entitiesInSquare[0];
    int damage = rollDamage();
    enemy->mHp -= damage;

    NotificationMessageRenderer::getInstance().queueMessage(
            "$(dwarf) hit " + enemy->mGraphic + " " + enemy->mName + "$[white] with $[red]$(heart)$[white]" + std::to_string(mHitTimes) +
            "d" + std::to_string(mHitAmount) + " for " + std::to_string(damage));

    // TODO: Add AV
    // TODO: Add avoidance
    if (enemy->getBehaviourByID("WanderBehaviour") != nullptr)
        enemy->getBehaviourByID("WanderBehaviour")->disable();
    if (enemy->getBehaviourByID("AttachmentBehaviour") != nullptr)
        enemy->getBehaviourByID("AttachmentBehaviour")->disable();
    if (enemy->getBehaviourByID("WanderAttachBehaviour") != nullptr)
        enemy->getBehaviourByID("WanderAttachBehaviour")->disable();
    if (enemy->getBehaviourByID("ChaseAndAttackBehaviour") != nullptr)
        enemy->getBehaviourByID("ChaseAndAttackBehaviour")->enable();

    auto& ui = dynamic_cast<StatusUIEntity&>(*EntityManager::getInstance().getEntityByID("StatusUI"));
    ui.setAttackTarget(enemy);

    if (enemy->mHp <= 0) {
        ui.clearAttackTarget();
        EntityManager::getInstance().queueForDeletion(enemy->mID);
        attacking = false;
        NotificationMessageRenderer::getInstance().queueMessage(enemy->mGraphic + " " + enemy->mName + "$[white] was ${black}$[red]destroyed!");
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

void PlayerEntity::handleInput(SDL_KeyboardEvent &e, bool &quit, std::unordered_map<ScreenType, std::shared_ptr<Screen>> &screens)
{
    auto key = e.keysym.sym;
    auto mod = e.keysym.mod;

    bool didAction = false;

    if (showingTooMuchWeightMessage) {
        if (key == SDLK_ESCAPE || key == SDLK_RETURN)
            showingTooMuchWeightMessage = false;
        return;
    }

    if (interactingWithEntity) {
        auto b = entityInteractingWith->getBehaviourByID("InteractableBehaviour");
        if (!dynamic_cast<InteractableBehaviour&>(*b).handleInput(e)) {
            entityInteractingWith = nullptr;
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
                    entityInteractingWith = entity;
                }
            }
        }

        // Handle looting
        if (key == SDLK_g) {
            auto entitiesAtPos = EntityManager::getInstance().getEntitiesAtPosFaster(mPos);

            // TODO: need to handle multiple items on same square properly
//            std::vector<std::shared_ptr<Entity>> waterEntities;

            std::vector<std::shared_ptr<Entity>> entitiesWithInventories;
            std::copy_if(entitiesAtPos.begin(), entitiesAtPos.end(), std::back_inserter(entitiesWithInventories),
                         [](auto &a) {
                return a->ID != "Player" && !a->isInventoryEmpty();
            });

            if (!entitiesWithInventories.empty()) {
                // TODO: what if multiple chests are present?
                auto entity = entitiesWithInventories[0];
                if (entity->mSkipLootingDialog) {
                    // Loot just the first item
                    if (addToInventory(entity->getInventoryItem(0))) {
                        entity->removeFromInventory(0);
                        didAction = true;
                        goto postLooting;
                    } else {
                        showingTooMuchWeightMessage = true;
                        return;
                    }
                } else {
                    auto &lootingDialog = dynamic_cast<LootingDialog&>(*screens[ScreenType::LOOTING]);
                    lootingDialog.showItemsToLoot(entity->getInventory(), entity);
                    return;
                }
            }

            std::vector<std::shared_ptr<Entity>> pickuppableEntities;
            std::copy_if(entitiesAtPos.begin(), entitiesAtPos.end(), std::back_inserter(pickuppableEntities),
                         [this](auto &a) {
                auto b = a->getBehaviourByID("PickuppableBehaviour");
                // Don't pick up if it isn't pickuppable, or if it is already in the player's inventory
                return (b != nullptr && !isInInventory(a->ID));
            });

            if (pickuppableEntities.empty())
                return;
            else if (pickuppableEntities.size() == 1) {
                if (addToInventory(*pickuppableEntities.begin())) {
                    didAction = true;
                    goto postLooting;
                } else {
                    showingTooMuchWeightMessage = true;
                    return;
                }
            } else {
                auto &lootingDialog = dynamic_cast<LootingDialog&>(*screens[ScreenType::LOOTING]);
                lootingDialog.showItemsToLoot(pickuppableEntities);
                return;
            }
        }

        postLooting:

        if (mHp > 0 && (key == SDLK_h || key == SDLK_j || key == SDLK_k || key == SDLK_l ||
                       key == SDLK_y || key == SDLK_u || key == SDLK_b || key == SDLK_n)) // Only ever attack if moving
        {
            Point posOffset;
            switch (key) {
                case SDLK_h:
                    posOffset = Point(-1, 0);
                    break;
                case SDLK_j:
                    posOffset = Point(0, 1);
                    break;
                case SDLK_k:
                    posOffset = Point(0, -1);
                    break;
                case SDLK_l:
                    posOffset = Point(1, 0);
                    break;
                case SDLK_y:
                    posOffset = Point(-1, -1);
                    break;
                case SDLK_u:
                    posOffset = Point(1, -1);
                    break;
                case SDLK_b:
                    posOffset = Point(-1, 1);
                    break;
                case SDLK_n:
                    posOffset = Point(1, 1);
                    break;
            }

            auto newPos = getPos() + posOffset;

            auto enemiesInSpace = EntityManager::getInstance().getEntitiesAtPosFaster(mPos + posOffset);
            // TODO: what if more than one enemy in space?
            if (!enemiesInSpace.empty()
                && ((enemiesInSpace[0]->getBehaviourByID("HostilityBehaviour") != nullptr
                     && enemiesInSpace[0]->getBehaviourByID("HostilityBehaviour")->isEnabled())
                    || (mod & KMOD_SHIFT)
                    || attacking)) {
                attack(newPos);
            } else {
                if (!moveTo(newPos))
                    return; // Don't tick if didn't move
            }

            didAction = true;
        }

        if (key == SDLK_i) {
            screens[ScreenType::INVENTORY]->enable();
        }

        if (key == SDLK_PERIOD) {
            EntityManager::getInstance().broadcast(SIGNAL_FORCE_WAIT);
            didAction = true;
        }

        if (key == SDLK_SEMICOLON) {
            auto &b = dynamic_cast<InspectionDialog&>(*screens[ScreenType::INSPECTION]);
            b.enableAtPoint(mPos);
        }

        if (key == SDLK_c)
            screens[ScreenType::CRAFTING]->enable();

        if (key == SDLK_e)
            screens[ScreenType::EQUIPMENT]->enable();

        if (key == SDLK_m)
            screens[ScreenType::NOTIFICATION]->enable();

        if (mod & KMOD_SHIFT && key == SDLK_SLASH)
            screens[ScreenType::HELP]->enable();
    }

    if (didAction)
        EntityManager::getInstance().tick();

    if (mHp <= 0 && key == SDLK_RETURN)
        quit = true;
}

void PlayerEntity::render(Font &font, Point currentWorldPos) {
    Entity::render(font, currentWorldPos);

    if (showingTooMuchWeightMessage) {
        const std::string& displayString = "You cannot carry that much!";
        MessageBoxRenderer::getInstance().queueMessageBoxCentered(displayString, 1);
    }

    if (interactingWithEntity) {
        dynamic_cast<InteractableBehaviour&>(*entityInteractingWith->getBehaviourByID("InteractableBehaviour")).render(font);
    }
}

bool PlayerEntity::addToInventory(const std::shared_ptr<Entity> &item) {
    if (Entity::addToInventory(item)) {
        NotificationMessageRenderer::getInstance()
                .queueMessage("You got a " + item->mGraphic + " " + item->mName + "${transparent}$[white]!");
        return true;
    }
    return false;
}

void PlayerEntity::addHunger(float hungerRestoration) {
    hunger = std::min(hunger + hungerRestoration, 1.0f);
}

void StatusUIEntity::render(Font &font, Point currentWorldPos) {
    std::string colorStr;
    float hpPercent = player.mHp / player.mMaxHp;

    if (hpPercent > 0.7)
        colorStr = "$[green]";
    else if (hpPercent > 0.3)
        colorStr = "$[yellow]";
    else
        colorStr = "$[red]";

    font.drawText("${black}" + colorStr + "hp " + std::to_string((int)ceil(player.mHp))
                  + "/" + std::to_string((int)ceil(player.mMaxHp)), SCREEN_WIDTH - X_OFFSET, 1);

    if (player.hunger > 0.7)
        font.drawText("${black}$[green]sated", SCREEN_WIDTH - X_OFFSET, 2);
    else if (player.hunger > 0.3)
        font.drawText("${black}$[yellow]hungry", SCREEN_WIDTH - X_OFFSET, 2);
    else
        font.drawText("${black}$[red]starving", SCREEN_WIDTH - X_OFFSET, 2);

    font.drawText("$[red]$(heart)$[white]" + std::to_string(player.mHitTimes) + "d" + std::to_string(player.computeMaxDamage())
            , SCREEN_WIDTH - X_OFFSET, 3);

    font.drawText("${black}" + std::to_string(player.getCarryingWeight()) + "/" + std::to_string(player.getMaxCarryWeight()) + "lb",
                  SCREEN_WIDTH - X_OFFSET, 4);

    if (forceTickDisplayTimer-- > 0) {
        font.drawText("Waited " + std::to_string(ticksWaitedDuringAnimation)
                      + " tick" + (ticksWaitedDuringAnimation > 1 ? "s" : "") + "...",
                      SCREEN_WIDTH - X_OFFSET - 8, SCREEN_HEIGHT - 2,
                      Color(0xFF, 0xFF, 0xFF, static_cast<Uint8>(static_cast<float>(forceTickDisplayTimer) / FORCE_TICK_DISPLAY_LENGTH * 0xFF)),
                      getColor("transparent"));
    } else {
        forceTickDisplayTimer = 0;
        ticksWaitedDuringAnimation = 1;
    }

    if (attackTarget != nullptr) {
        float enemyhpPercent = attackTarget->mHp / attackTarget->mMaxHp;
        std::string enemyhpString = "${black}";

        if (enemyhpPercent == 1)
            enemyhpString += "$[white]unscathed";
        else if (enemyhpPercent > 0.7)
            enemyhpString += "$[green]moderate";
        else if (enemyhpPercent > 0.3)
            enemyhpString += "$[yellow]badly wounded";
        else
            enemyhpString += "$[red]near death";

        font.drawText(enemyhpString, SCREEN_WIDTH - X_OFFSET - 2, 6);
    }

    // Drop attack target after 10 turns of inactivity
    if (attackTargetTimer == 0)
        attackTarget = nullptr;

    font.drawText(EntityManager::getInstance().getTimeOfDay().toWordString(), SCREEN_WIDTH - X_OFFSET, 8);
}

void StatusUIEntity::emit(uint32_t signal) {
    if (signal & SIGNAL_FORCE_WAIT) {
        if (forceTickDisplayTimer > 0) {
            ticksWaitedDuringAnimation++;
        }
        forceTickDisplayTimer = FORCE_TICK_DISPLAY_LENGTH;
    }

    Entity::emit(signal);
}

void StatusUIEntity::tick() {
    if (attackTarget != nullptr &&
        attackTarget->getBehaviourByID("ChaseAndAttackBehaviour") != nullptr &&
        !attackTarget->getBehaviourByID("ChaseAndAttackBehaviour")->isEnabled())
    {
        attackTargetTimer--;
    }

    Entity::tick();
}

void CatEntity::destroy() {
    auto corpse = std::make_shared<CorpseEntity>(mID + "corpse", 0.4, mName, 100);
    corpse->setPos(getPos());
    EntityManager::getInstance().addEntity(corpse);
}

void WolfEntity::destroy() {
    auto corpse = std::make_shared<CorpseEntity>(mID + "corpse", 0.4, mName, 100);
    corpse->setPos(getPos());
    EntityManager::getInstance().addEntity(corpse);
}

void BushEntity::render(Font &font, Point currentWorldPos) {
    if (!isInventoryEmpty()) {
        mGraphic = "${black}$[purple]$(div)";
    } else {
        mGraphic = "${black}$[green]$(div)";
    }

    Entity::render(font, currentWorldPos);
}

void FireEntity::render(Font &font, Point currentWorldPos) {
    if (fireLevel < 0.1)
        mGraphic = "${black}$[grey]%";
    else if (rand() % 2 == 0)
        mGraphic = "${black}$[red]%";
    else
        mGraphic = "${black}$[orange]%";

    dynamic_cast<LightEmittingBehaviour&>(*getBehaviourByID("LightEmittingBehaviour")).setRadius(static_cast<int>(std::round(6 * fireLevel)));

    Entity::render(font, currentWorldPos);
}

void FireEntity::tick() {
    fireLevel -= 0.005f;
}

void GrassEntity::render(Font &font, Point currentWorldPos) {
    if (!isInventoryEmpty()) {
        mGraphic = "${black}$[grasshay]$(tau)";
    } else {
        mGraphic = "${black}$[grasshay].";
    }

    Entity::render(font, currentWorldPos);
}

bool FireEntity::RekindleBehaviour::handleInput(SDL_KeyboardEvent &e) {
    switch (e.keysym.sym) {
        case SDLK_j:
            if (choosingItemToUse) {
                const auto &player = EntityManager::getInstance().getEntityByID("Player");
                const auto &entities = player->filterInventoryForCraftingMaterials(std::vector<std::string> {"grass", "wood"});

                if (choosingItemIndex == entities.size() - 1)
                    choosingItemIndex = 0;
                else
                    ++choosingItemIndex;
            }
            break;
        case SDLK_k:
            if (choosingItemToUse) {
                const auto &player = EntityManager::getInstance().getEntityByID("Player");
                const auto &entities = player->filterInventoryForCraftingMaterials(std::vector<std::string> {"grass", "wood"});

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
                const auto &entities = player->filterInventoryForCraftingMaterials(std::vector<std::string> {"grass", "wood"});
                player->removeFromInventory(entities[choosingItemIndex]);
                dynamic_cast<FireEntity&>(mParent).fireLevel = 1;
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

void FireEntity::RekindleBehaviour::render(Font &font) {
    MessageBoxRenderer::getInstance().queueMessageBoxCentered("$(right)Rekindle", 1);

    if (choosingItemToUse) {
        const auto &player = EntityManager::getInstance().getEntityByID("Player");
        const auto &entities = player->filterInventoryForCraftingMaterials(std::vector<std::string> {"grass", "wood"});

        if (entities.empty()) {
            choosingItemToUse = false;
            return;
        }

        std::vector<std::string> displayStrings;

        for (int i = 0; i < entities.size(); ++i) {
            const auto &entity = EntityManager::getInstance().getEntityByID(entities[i]);
            displayStrings.emplace_back((i == choosingItemIndex ? "$(right)" : " ") + entity->mGraphic + " " + entity->mName);
        }

        MessageBoxRenderer::getInstance().queueMessageBoxCentered(displayStrings, 1);
    }
}

void GlowbugEntity::render(Font &font, Point currentWorldPos) {
    static int timer = 0;

    if (timer++ > (rand() % 20) + 20){
        switch (rand() % 3) {
            case 0:
                mGraphic = "$[green]`";
                break;
            case 1:
                mGraphic = "$[green]'";
                break;
            case 2:
                mGraphic = "";
                break;
            default:
                break;
        }

        timer = 0;
    }

    Entity::render(font, currentWorldPos);
}
