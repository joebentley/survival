#include "entities.h"
#include "world.h"

#include <algorithm>

bool PlayerEntity::attack(const Point &attackPos) {
    auto entitiesInSquare = manager.getEntitiesAtPos(attackPos);

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

    auto& ui = dynamic_cast<StatusUIEntity&>(*manager.getEntityByID("StatusUI"));
    ui.setAttackTarget(enemy);

    if (enemy->hp <= 0) {
        ui.attackTarget = nullptr;
        manager.queueForDeletion(enemy->ID);
        attacking = false;
        std::cout << enemy->name << " was destroyed!" << "\n";

        std::shared_ptr<Entity> corpse = std::make_shared<CorpseEntity>(manager, enemy->ID + "corpse", 0.4, enemy->name);
        corpse->pos = enemy->pos;
        manager.addEntity(corpse);
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

void PlayerEntity::handleInput(SDL_KeyboardEvent &e, bool &quit, InventoryScreen &inventoryScreen, LootingDialog &lootingDialog) {
    auto key = e.keysym.sym;
    auto mod = e.keysym.mod;

    bool didAction = false;

    if (hp > 0) {
        if (key == SDLK_e) {
            auto entitiesAtPos = manager.getEntitiesAtPos(pos);
            auto it = std::find_if(entitiesAtPos.begin(), entitiesAtPos.end(),
                                   [](auto &a) { return a->canBePickedUp && a->hasBehaviour("EatableBehaviour"); });
            if (it == entitiesAtPos.end())
                return;
            else {
                manager.queueForDeletion((*it)->ID);
                hunger = std::min(hunger + dynamic_cast<EatableBehaviour&>(*((*it)->getBehaviourByID("EatableBehaviour"))).hungerRestoration, 1.0);
                didAction = true;
                return;
            }
        }

        if (key == SDLK_g) {
            auto entitiesAtPos = manager.getEntitiesAtPos(pos);
            std::vector<std::shared_ptr<Entity>> pickuppableEntities;
            std::copy_if(entitiesAtPos.begin(), entitiesAtPos.end(),
                         std::back_inserter(pickuppableEntities), [](auto &a) { return a->canBePickedUp; });

            if (pickuppableEntities.empty())
                return;
            else if (pickuppableEntities.size() == 1) {
                addToInventory(*pickuppableEntities.begin());
                didAction = true;
                return;
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

            auto enemiesInSpace = manager.getEntitiesAtPos(pos + posOffset);
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
            manager.broadcast(SIGNAL_FORCE_WAIT);
            didAction = true;
        }

        if (didAction)
            manager.tick();
    }

    if (hp <= 0 && key == SDLK_RETURN)
        quit = true;
}

void StatusUIEntity::render(Font &font, int currentWorldX, int currentWorldY) {
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

        font.drawText(enemyhpString, SCREEN_WIDTH - X_OFFSET - 2, 4);
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
