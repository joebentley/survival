#pragma once

#include "../../Properties.h"
#include "../Entity.h"

struct WaterskinEntity : Entity {
    explicit WaterskinEntity() : Entity("", "Waterskin", "$[brown]$(male)") {
        addProperty(std::make_unique<WaterContainerProperty>());
        addProperty(std::make_unique<PickuppableProperty>(1));
    }
};
