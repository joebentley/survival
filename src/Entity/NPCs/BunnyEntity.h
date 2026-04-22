#pragma once

#include "../../Behaviour/AI/SeekHomeBehaviour.h"
#include "../../Behaviour/AI/WanderAttachBehaviour.h"
#include "../Entity.h"

struct BunnyEntity : Entity {
    explicit BunnyEntity() : Entity("", "Bunny", "$(bunny1)", 10.0f, 10.0f, 0.05f) {
        addBehaviour(std::make_unique<WanderAttachBehaviour>(*this, 0.5, 0.5, 0.1));
        addBehaviour(std::make_unique<SeekHomeBehaviour>(*this, "Bunny's House"));
    }

    void render(Font &font, Point currentWorldPos) override;

    bool isInHome() const;
};