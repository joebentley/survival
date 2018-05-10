#include "entities.h"
#include "world.h"

void PlayerEntity::attack(const Point &attackPos) {
    auto entitiesInSquare = manager.getEntitiesAtPos(attackPos);

    if (entitiesInSquare.empty()) {
        return;
    }

    auto enemy = entitiesInSquare[0];
    enemy->hp--;

    // TODO: Make enemy recover their movement after brief period
    if (enemy->getBehaviourByID("WanderBehaviour") != nullptr)
        enemy->getBehaviourByID("WanderBehaviour")->enabled = false;
    if (enemy->getBehaviourByID("AttachmentBehaviour") != nullptr)
        enemy->getBehaviourByID("AttachmentBehaviour")->enabled = false;

    auto& ui = dynamic_cast<StatusUIEntity&>(*manager.getByID("StatusUI"));
    ui.attackTarget = enemy;

    if (enemy->hp <= 0) {
        ui.attackTarget = nullptr;
        manager.queueForDeletion(enemy->ID);
    }
}

void StatusUIEntity::render(Font &font, int currentWorldX, int currentWorldY) {
    std::string colorStr;
    double hpPercent = static_cast<double>(player.hp) / player.maxhp;

    if (hpPercent > 0.7)
        colorStr = "$[green]";
    else if (hpPercent > 0.3)
        colorStr = "$[yellow]";
    else
        colorStr = "$[red]";

    font.drawText("${black}" + colorStr + "hp " + std::to_string(player.hp) + "/" + std::to_string(player.maxhp), SCREEN_WIDTH - X_OFFSET, 1);

    if (player.hunger > 7)
        font.drawText("${black}$[green]sated", SCREEN_WIDTH - X_OFFSET, 2);
    else if (player.hunger > 3)
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
