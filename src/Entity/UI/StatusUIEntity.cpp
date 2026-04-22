#include "StatusUIEntity.h"

#include "../../flags.h"

void StatusUIEntity::render(Font &font, Point /*currentWorldPos*/) {
    std::string colorStr;
    float hpPercent = player.mHp / player.mMaxHp;

    if (hpPercent > 0.7)
        colorStr = "$[green]";
    else if (hpPercent > 0.3)
        colorStr = "$[yellow]";
    else
        colorStr = "$[red]";

    font.drawText("${black}" + colorStr + "hp " + std::to_string((int)ceil(player.mHp)) + "/" +
                      std::to_string((int)ceil(player.mMaxHp)),
                  World::SCREEN_WIDTH - X_OFFSET, 1);

    if (player.hunger > 0.7)
        font.drawText("${black}$[green]sated", World::SCREEN_WIDTH - X_OFFSET, 2);
    else if (player.hunger > 0.3)
        font.drawText("${black}$[yellow]hungry", World::SCREEN_WIDTH - X_OFFSET, 2);
    else
        font.drawText("${black}$[red]starving", World::SCREEN_WIDTH - X_OFFSET, 2);

    font.drawText("$[red]$(heart)$[white]" + std::to_string(player.mHitTimes) + "d" +
                      std::to_string(player.computeMaxDamage()),
                  World::SCREEN_WIDTH - X_OFFSET, 3);

    font.drawText("${black}" + std::to_string(player.getCarryingWeight()) + "/" +
                      std::to_string(player.getMaxCarryWeight()) + "lb",
                  World::SCREEN_WIDTH - X_OFFSET, 4);

    if (forceTickDisplayTimer-- > 0) {
        font.drawText(
            "Waited " + std::to_string(ticksWaitedDuringAnimation) + " tick" +
                (ticksWaitedDuringAnimation > 1 ? "s" : "") + "...",
            World::SCREEN_WIDTH - X_OFFSET - 8, World::SCREEN_HEIGHT - 2,
            Color(0xFF, 0xFF, 0xFF,
                  static_cast<Uint8>(static_cast<float>(forceTickDisplayTimer) / FORCE_TICK_DISPLAY_LENGTH * 0xFF)),
            Color::getColor("transparent"));
    } else {
        forceTickDisplayTimer = 0;
        ticksWaitedDuringAnimation = 1;
    }

    if (!mAttackTargetID.empty()) {
        auto attackTarget = EntityManager::getInstance().getEntityByID(mAttackTargetID);
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

        font.drawText(enemyhpString, World::SCREEN_WIDTH - X_OFFSET - 2, 6);
    }

    // Drop attack target after 10 turns of inactivity
    if (attackTargetTimer == 0)
        mAttackTargetID.clear();

    font.drawText(EntityManager::getInstance().getTimeOfDay().toWordString(), World::SCREEN_WIDTH - X_OFFSET, 8);
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
    if (!mAttackTargetID.empty()) {
        auto attackTarget = EntityManager::getInstance().getEntityByID(mAttackTargetID);
        if (attackTarget->getBehaviourByID("ChaseAndAttackBehaviour") != nullptr &&
            !attackTarget->getBehaviourByID("ChaseAndAttackBehaviour")->isEnabled()) {
            attackTargetTimer--;
        }
    }

    Entity::tick();
}
