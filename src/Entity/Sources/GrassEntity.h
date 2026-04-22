#pragma once

#include "../Entity.h"

struct GrassEntity : Entity {
    const int RESTOCK_RATE = 100; // ticks

    const std::string SHORT_DESC = "It is dry grass";
    const std::string LONG_DESC = "You can harvest it";

    explicit GrassEntity(std::string ID = "");

    void render(Font &font, Point currentWorldPos) override;
};
