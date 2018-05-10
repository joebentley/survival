#include "entities.h"
#include "world.h"

void HealthHungerUIEntity::render(Font &font, int currentWorldX, int currentWorldY) {
    std::string colorStr;
    double hpPercent = static_cast<double>(player.hp) / player.maxhp;

    if (hpPercent > 0.7)
        colorStr = "$[green]";
    else if (hpPercent > 0.3)
        colorStr = "$[yellow]";
    else
        colorStr = "$[red]";

    font.drawText("${black}" + colorStr + "hp " + std::to_string(player.hp) + "/" + std::to_string(player.maxhp), SCREEN_WIDTH - 10, 1);

    if (player.hunger > 7)
        font.drawText("${black}$[green]sated", SCREEN_WIDTH - 10, 2);
    else if (player.hunger > 3)
        font.drawText("${black}$[yellow]hungry", SCREEN_WIDTH - 10, 2);
    else
        font.drawText("${black}$[red]starving", SCREEN_WIDTH - 10, 2);
}