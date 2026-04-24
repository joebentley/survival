#include "WaterskinEntity.h"

#include "../../Properties.h"

WaterskinEntity::WaterskinEntity() : Entity("", "Waterskin", "$[brown]$(male)") {
    addProperty(std::make_unique<WaterContainerProperty>());
    addProperty(std::make_unique<PickuppableProperty>(1));
}