#include "GlowbugEntity.h"

void GlowbugEntity::render(Font &font, Point currentWorldPos) {
    static int timer = 0;

    if (timer++ > (rand() % 20) + 20) {
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