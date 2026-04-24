#pragma once

#include "../Entity.h"

struct GlowbugEntity : Entity {
    explicit GlowbugEntity(std::string ID = "");

    void render(Font &font, Point currentWorldPos) override;
};
