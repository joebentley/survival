#pragma once

#include "../Entity.h"

struct BunnyEntity : Entity {
    explicit BunnyEntity();

    void render(Font &font, Point currentWorldPos) override;

    bool isInHome() const;
};