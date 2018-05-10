#include "entities.h"
#include "world.h"

void HealthUIEntity::render(Font &font, int currentWorldX, int currentWorldY) {
    font.drawText("hp " + std::to_string(player.hp) + "/" + std::to_string(player.maxhp), SCREEN_WIDTH - 10, 1);
}