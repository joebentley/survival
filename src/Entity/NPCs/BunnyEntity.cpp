#include "BunnyEntity.h"

#include "../../Behaviour/AI/SeekHomeBehaviour.h"
#include "../../Behaviour/AI/WanderAttachBehaviour.h"

BunnyEntity::BunnyEntity() : Entity("", "Bunny", "$(bunny1)", 10.0f, 10.0f, 0.05f) {
    addBehaviour(std::make_unique<WanderAttachBehaviour>(*this, 0.5, 0.5, 0.1));
    addBehaviour(std::make_unique<SeekHomeBehaviour>(*this, "Bunny's House"));
}

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
