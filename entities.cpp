#include "entities.h"
#include "world.h"

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
}

void StatusUIEntity::emit(const std::string &event) {
    if (event == "force tick") {
        if (forceTickDisplayTimer > 0) {
            ticksWaitedDuringAnimation++;
        }

        forceTickDisplayTimer = 100;
    }

    Entity::emit(event);
}

