#pragma once

#include "../Entity.h"

struct WolfEntity : Entity {
    explicit WolfEntity(std::string ID = "");

    void destroy() override;
};
