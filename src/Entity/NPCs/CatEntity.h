#pragma once

#include "../Entity.h"

struct CatEntity : Entity {
    explicit CatEntity(std::string ID = "");

    void destroy() override;
};