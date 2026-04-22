#pragma once
#include "../Entity.h"

struct BushEntity : Entity {
    const int RESTOCK_RATE = 200; // ticks

    const std::string SHORT_DESC = "It's a bush!";
    const std::string LONG_DESC = "";

    explicit BushEntity(std::string ID = "");

    void render(Font &font, Point currentWorldPos) override;
};
