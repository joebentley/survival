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
        enemy->getBehaviourByID("WanderBehaviour")->enabled = false;
    if (enemy->getBehaviourByID("AttachmentBehaviour") != nullptr)
        enemy->getBehaviourByID("AttachmentBehaviour")->enabled = false;
    if (enemy->getBehaviourByID("WanderAttachBehaviour") != nullptr)
        enemy->getBehaviourByID("WanderAttachBehaviour")->enabled = false;
    if (enemy->getBehaviourByID("ChaseAndAttackBehaviour") != nullptr)
        enemy->getBehaviourByID("ChaseAndAttackBehaviour")->enabled = true;

    auto& ui = dynamic_cast<StatusUIEntity&>(*EntityManager::getInstance().getEntityByID("StatusUI"));
    ui.setAttackTarget(enemy);

    if (enemy->hp <= 0) {
        ui.attackTarget = nullptr;
        EntityManager::getInstance().queueForDeletion(enemy->ID);
        attacking = false;
        std::cout << enemy->name << " was destroyed!" << "\n";
        enemy->destroy();

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

void PlayerEntity::handleInput(SDL_KeyboardEvent &e, bool &quit, InventoryScreen &inventoryScreen, LootingDialog &lootingDialog, InspectionDialog &inspectionDialog) {
    auto key = e.keysym.sym;
    auto mod = e.keysym.mod;

    bool didAction = false;

    if (showingTooMuchWeightMessage) {
        if (key == SDLK_ESCAPE || key == SDLK_RETURN)
            showingTooMuchWeightMessage = false;
        return;
    }

    if (hp > 0) {
        if (key == SDLK_e) {
            auto entitiesAtPos = EntityManager::getInstance().getEntitiesAtPos(pos);
            auto it = std::find_if(entitiesAtPos.begin(), entitiesAtPos.end(),
                                   [](auto &a) { return a->hasBehaviour("EatableBehaviour"); });
            if (it == entitiesAtPos.end())
                return;
            else {
                EntityManager::getInstance().queueForDeletion((*it)->ID);
                hunger = std::min(hunger + dynamic_cast<EatableBehaviour&>(*((*it)->getBehaviourByID("EatableBehaviour"))).hungerRestoration, 1.0);
                didAction = true;
            }
        }

        if (key == SDLK_g) {
            auto entitiesAtPos = EntityManager::getInstance().getEntitiesAtPos(pos);

            std::vector<std::shared_ptr<Entity>> entitiesWithInventories;
            std::copy_if(entitiesAtPos.begin(), entitiesAtPos.end(), std::back_inserter(entitiesWithInventories),
                         [](auto &a) {
                return a->ID != "Player" && !a->inventory.empty();
            });

            if (!entitiesWithInventories.empty()) {
                // TODO: what if multiple chests are present?
                lootingDialog.showItemsToLoot(entitiesWithInventories[0]->inventory, entitiesWithInventories[0]);
                return;
            }

            std::vector<std::shared_ptr<Entity>> pickuppableEntities;
            std::copy_if(entitiesAtPos.begin(), entitiesAtPos.end(), std::back_inserter(pickuppableEntities),
                         [this](auto &a) {
                auto b = a->getBehaviourByID("PickuppableBehaviour");
                // Don't pick up if it isn't pickuppable, or if it is already in the player's inventory
                return (b != nullptr && std::find(this->inventory.cbegin(), this->inventory.cend(), a) == this->inventory.cend());
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

            auto enemiesInSpace = EntityManager::getInstance().getEntitiesAtPos(pos + posOffset);
            // TODO: what if more than one enemy in space?
            if (!enemiesInSpace.empty()
                && ((enemiesInSpace[0]->getBehaviourByID("HostilityBehaviour") != nullptr
                     && enemiesInSpace[0]->getBehaviourByID("HostilityBehaviour")->enabled)
                    || (mod & KMOD_SHIFT)
                    || attacking)) {
                attack(pos + posOffset);
            } else
                pos += posOffset;

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

void StatusUIEntity::render(Font &font, Point currentWorldPos) {
    std::string colorStr;
    double hpPercent = player.hp / player.maxhp;

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

    font.drawText("${black}" + std::to_string(player.getCarryingWeight()) + "/" + std::to_string(player.maxCarryWeight) + "lb",
                  SCREEN_WIDTH - X_OFFSET, 4);

    if (forceTickDisplayTimer-- > 0) {
        font.drawText("Waited " + std::to_string(ticksWaitedDuringAnimation)
                      + " tick" + (ticksWaitedDuringAnimation > 1 ? "s" : "") + "...",
                      SCREEN_WIDTH - X_OFFSET - 8, SCREEN_HEIGHT - 2,
                      Color(0xFF, 0xFF, 0xFF, static_cast<Uint8>(static_cast<double>(forceTickDisplayTimer) / 100 * 0xFF)), getColor("transparent"));
    } else {
        forceTickDisplayTimer = 0;
        ticksWaitedDuringAnimation = 1;
    }

    if (attackTarget != nullptr) {
        double enemyhpPercent = attackTarget->hp / attackTarget->maxhp;
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
}

void StatusUIEntity::emit(uint32_t signal) {
    if (signal & SIGNAL_FORCE_WAIT) {
        if (forceTickDisplayTimer > 0) {
            ticksWaitedDuringAnimation++;
        }

        forceTickDisplayTimer = 100;
    }

    Entity::emit(signal);
}

void StatusUIEntity::tick() {
    if (attackTarget != nullptr &&
        attackTarget->getBehaviourByID("ChaseAndAttackBehaviour") != nullptr &&
        !attackTarget->getBehaviourByID("ChaseAndAttackBehaviour")->enabled)
    {
        attackTargetTimer--;
    }

    Entity::tick();
}

void CatEntity::destroy() {
    auto corpse = std::make_shared<CorpseEntity>(ID + "corpse", 0.4, name, 100);
    corpse->pos = pos;
    EntityManager::getInstance().addEntity(corpse);
}

void BushEntity::render(Font &font, Point currentWorldPos) {
    if (!inventory.empty()) {
        graphic = "${black}$[purple]$(div)";
    } else {
        graphic = "${black}$[green]$(div)";
    }

    Entity::render(font, currentWorldPos);
}
