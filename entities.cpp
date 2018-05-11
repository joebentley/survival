#include "entities.h"
#include "world.h"

bool PlayerEntity::attack(const Point &attackPos) {
    auto entitiesInSquare = manager.getEntitiesAtPos(attackPos);

    if (entitiesInSquare.empty()) {
        return false;
    }

    auto enemy = entitiesInSquare[0];
    int damage = rollDamage();
    enemy->hp -= damage;

    std::cout << "Player hit " << enemy->ID << " with " << hitTimes << "d" << hitAmount << " for " << damage << "\n";

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

    auto& ui = dynamic_cast<StatusUIEntity&>(*manager.getByID("StatusUI"));
    ui.attackTarget = enemy;

    if (enemy->hp <= 0) {
        ui.attackTarget = nullptr;
        manager.queueForDeletion(enemy->ID);
        attacking = false;
        std::cout << enemy->ID << " was destroyed!" << "\n";
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

void PlayerEntity::emit(Uint32 signal) {
    Entity::emit(signal);

    if ((signal & SIGNAL_INPUT_UP || signal & SIGNAL_INPUT_DOWN || signal & SIGNAL_INPUT_LEFT || signal & SIGNAL_INPUT_RIGHT)) // Only ever attack if moving
    {
        Point posOffset;
        if (signal & SIGNAL_INPUT_UP)
            posOffset.y = -1;
        else if (signal & SIGNAL_INPUT_DOWN)
            posOffset.y = 1;
        if (signal & SIGNAL_INPUT_LEFT)
            posOffset.x = -1;
        else if (signal & SIGNAL_INPUT_RIGHT)
            posOffset.x = 1;

        auto enemiesInSpace = manager.getEntitiesAtPos(pos + posOffset);
        // TODO: what if more than one enemy in space?
        if (!enemiesInSpace.empty()
            && ((enemiesInSpace[0]->getBehaviourByID("HostilityBehaviour") != nullptr
                 && enemiesInSpace[0]->getBehaviourByID("HostilityBehaviour")->enabled)
                || (signal & SIGNAL_FORCE_ATTACK)
                || attacking))
        {
            attack(pos + posOffset);
        } else
            pos += posOffset;
    }

    if (!(signal & SIGNAL_FORCE_WAIT))
        manager.tick(); // Only tick on player movement
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
        double enemyhpPercent = static_cast<double>(attackTarget->hp) / attackTarget->maxhp;
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
