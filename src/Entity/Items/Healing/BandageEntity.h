#pragma once

#include "../../Entity.h"

struct BandageEntity : Entity {
    const std::string SHORT_DESC = "A rudimentary bandage made of grass";

    explicit BandageEntity(std::string ID = "");
};
