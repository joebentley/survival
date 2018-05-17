#include "entities.h"
#include "world.h"

#include <algorithm>

bool PlayerEntity::attack(const Point &attackPos) {
    auto entitiesInSquare = EntityManager::getInstance().getEntitiesAtPos(attackPos);

    if (entitiesInSquare.empty()) {
        return false;
    }

    auto enemy = entitiesInSquare[0];
    int damage = rollDamage();
    enemy->hp -= damage;

    std::cout << "Player hit " << enemy->name << " with " << hitTimes << "d" << hitAmount << " for " << damage << "\n";

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

    if (enemy->hp <= 0) {
        ui.clearAttackTarget();
        EntityManager::getInstance().queueForDeletion(enemy->ID);
        attacking = false;
        std::cout << enemy->name << " was destroyed!" << "\n";

        return false;
    }

    attacking = true;
    return true;
}

void PlayerEntity::tick() {
    hunger -= hungerRate;

    if (hunger < 0.3)
        hp -= hungerDamageRate;

    Entity::tick();
}

void PlayerEntity::handleInput(SDL_KeyboardEvent &e, bool &quit, InventoryScreen &inventoryScreen,
                               LootingDialog &lootingDialog, InspectionDialog &inspectionDialog,
                               CraftingScreen &craftingScreen, EquipmentScreen &equipmentScreen) {
    auto key = e.keysym.sym;
    auto mod = e.keysym.mod;

    bool didAction = false;

    if (showingTooMuchWeightMessage) {
        if (key == SDLK_ESCAPE || key == SDLK_RETURN)
            showingTooMuchWeightMessage = false;
        return;
    }

    if (hp > 0) {
        // Handle eating from ground
//        if (key == SDLK_e) {
//            auto entitiesAtPos = EntityManager::getInstance().getEntitiesAtPos(pos);
//            auto it = std::find_if(entitiesAtPos.begin(), entitiesAtPos.end(),
//                                   [](auto &a) { return a->hasBehaviour("EatableBehaviour"); });
//            if (it == entitiesAtPos.end())
//                return;
//            else {
//                EntityManager::getInstance().queueForDeletion((*it)->ID);
//                addHunger(dynamic_cast<EatableBehaviour&>(*((*it)->getBehaviourByID("EatableBehaviour"))).hungerRestoration);
//                didAction = true;
//            }
//        }

        // Handle looting
        if (key == SDLK_g) {
            auto entitiesAtPos = EntityManager::getInstance().getEntitiesAtPos(pos);

            std::vector<std::shared_ptr<Entity>> entitiesWithInventories;
            std::copy_if(entitiesAtPos.begin(), entitiesAtPos.end(), std::back_inserter(entitiesWithInventories),
                         [](auto &a) {
                return a->ID != "Player" && !a->isInventoryEmpty();
            });

            if (!entitiesWithInventories.empty()) {
                // TODO: what if multiple chests are present?
                auto entity = entitiesWithInventories[0];
                if (entity->skipLootingDialog) {
                    // Loot just the first item
                    if (addToInventory(entity->getInventoryItem(0))) {
                        entity->removeFromInventory(0);
                        didAction = true;
                    } else {
                        showingTooMuchWeightMessage = true;
                        return;
                    }
                } else {
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
                } else {
                    showingTooMuchWeightMessage = true;
                    return;
                }
            } else {
                lootingDialog.showItemsToLoot(pickuppableEntities);
                return;
            }
        }

        if (hp > 0 && (key == SDLK_h || key == SDLK_j || key == SDLK_k || key == SDLK_l ||
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

            // Don't allow moving off edge of world
            auto newPos = getPos() + posOffset;
            if (newPos.x < 0 || newPos.y < 0 || newPos.x >= SCREEN_WIDTH * WORLD_WIDTH || newPos.y >= SCREEN_HEIGHT * WORLD_HEIGHT)
                return;

            auto enemiesInSpace = EntityManager::getInstance().getEntitiesAtPos(pos + posOffset);
            // TODO: what if more than one enemy in space?
            if (!enemiesInSpace.empty()
                && ((enemiesInSpace[0]->getBehaviourByID("HostilityBehaviour") != nullptr
                     && enemiesInSpace[0]->getBehaviourByID("HostilityBehaviour")->isEnabled())
                    || (mod & KMOD_SHIFT)
                    || attacking)) {
                attack(newPos);
            } else {
                auto oldWorldPos = getWorldPos();
                auto moved = moveTo(newPos);
                if (!moved)
                    return; // Don't tick if didn't move
                else {
                    // Check if we moved to a new world coordinate, if so update current entities on world
                    if (oldWorldPos != getWorldPos())
                        EntityManager::getInstance().recomputeCurrentEntitiesOnScreen(getWorldPos());
                }
            }

            didAction = true;
        }

        if (key == SDLK_i) {
            inventoryScreen.chosenIndex = 0;
            inventoryScreen.enabled = true;
        }

        if (key == SDLK_PERIOD) {
            EntityManager::getInstance().broadcast(SIGNAL_FORCE_WAIT);
            didAction = true;
        }

        if (key == SDLK_SEMICOLON) {
            inspectionDialog.enabled = true;
            inspectionDialog.chosenPoint = pos;
        }

        if (key == SDLK_c)
            craftingScreen.enable();

        if (key == SDLK_e)
            equipmentScreen.enable();

        if (didAction)
            EntityManager::getInstance().tick();
    }

    if (hp <= 0 && key == SDLK_RETURN)
        quit = true;
}

void PlayerEntity::render(Font &font, Point currentWorldPos) {
    Entity::render(font, currentWorldPos);

    if (showingTooMuchWeightMessage) {
        const std::string& displayString = "You cannot carry that much!";
        showMessageBox(font, displayString, 20, 10);
    }
}

bool PlayerEntity::addToInventory(const std::shared_ptr<Entity> &item) {
    if (Entity::addToInventory(item)) {
        auto &ui = dynamic_cast<StatusUIEntity&>(*EntityManager::getInstance().getEntityByID("StatusUI"));
        ui.showLootedItemNotification(item->graphic + " " + item->name);
        return true;
    }
    return false;
}

void PlayerEntity::addHunger(float hungerRestoration) {
    hunger = std::min(hunger + hungerRestoration, 1.0f);
}

void StatusUIEntity::render(Font &font, Point currentWorldPos) {
    std::string colorStr;
    float hpPercent = player.hp / player.maxhp;

    if (hpPercent > 0.7)
        colorStr = "$[green]";
    else if (hpPercent > 0.3)
        colorStr = "$[yellow]";
    else
        colorStr = "$[red]";

    font.drawText("${black}" + colorStr + "hp " + std::to_string((int)ceil(player.hp))
                  + "/" + std::to_string((int)ceil(player.maxhp)), SCREEN_WIDTH - X_OFFSET, 1);

    if (player.hunger > 0.7)
        font.drawText("${black}$[green]sated", SCREEN_WIDTH - X_OFFSET, 2);
    else if (player.hunger > 0.3)
        font.drawText("${black}$[yellow]hungry", SCREEN_WIDTH - X_OFFSET, 2);
    else
        font.drawText("${black}$[red]starving", SCREEN_WIDTH - X_OFFSET, 2);

    font.drawText("$[red]$(heart)$[white]" + std::to_string(player.hitTimes) + "d" + std::to_string(player.computeMaxDamage())
            , SCREEN_WIDTH - X_OFFSET, 3);

    font.drawText("${black}" + std::to_string(player.getCarryingWeight()) + "/" + std::to_string(player.maxCarryWeight) + "lb",
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

    if (showLootedItemDisplayTimer-- > 0) {
        auto alpha = static_cast<int>(static_cast<float>(showLootedItemDisplayTimer) / SHOW_LOOTED_DISPLAY_LENGTH * 0xFF);
        font.drawText("You got a " + showLootedItemString, 3, SCREEN_HEIGHT - 2, alpha);
    }

    if (attackTarget != nullptr) {
        float enemyhpPercent = attackTarget->hp / attackTarget->maxhp;
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

void StatusUIEntity::showLootedItemNotification(std::string itemString) {
    showLootedItemString = std::move(itemString);
    showLootedItemDisplayTimer = SHOW_LOOTED_DISPLAY_LENGTH;
}

void CatEntity::destroy() {
    auto corpse = std::make_shared<CorpseEntity>(ID + "corpse", 0.4, name, 100);
    corpse->setPos(getPos());
    EntityManager::getInstance().addEntity(corpse);
}

void BushEntity::render(Font &font, Point currentWorldPos) {
    if (!isInventoryEmpty()) {
        graphic = "${black}$[purple]$(div)";
    } else {
        graphic = "${black}$[green]$(div)";
    }

    Entity::render(font, currentWorldPos);
}

void FireEntity::render(Font &font, Point currentWorldPos) {
    if (rand() % 2 == 0)
        graphic = "${black}$[red]%";
    else
        graphic = "${black}$[orange]%";

    Entity::render(font, currentWorldPos);
}

void GrassEntity::render(Font &font, Point currentWorldPos) {
    if (!isInventoryEmpty()) {
        graphic = "${black}$[grasshay]$(tau)";
    } else {
        graphic = "${black}$[grasshay].";
    }

    Entity::render(font, currentWorldPos);
}
