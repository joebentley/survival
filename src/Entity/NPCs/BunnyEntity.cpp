#include "BunnyEntity.h"

void BunnyEntity::render(Font &font, Point currentWorldPos) {
    // animate the bunny's sprite
    static int i = 0;
    i++;

    mGraphic = i > 30 ? "$(bunny1)" : "$(bunny2)";

    if (i > 60)
        i = 0;

    // if in a home right now, don't render the bunny
    if (isInHome())
        return;

    Entity::render(font, currentWorldPos);
}

bool BunnyEntity::isInHome() const {
    auto b = getBehaviourByID("SeekHomeBehaviour");

    if (b != nullptr) {
        return dynamic_cast<SeekHomeBehaviour *>(b)->isInHome;
    }

    return false;
}
