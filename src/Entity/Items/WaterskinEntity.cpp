#include "WaterskinEntity.h"

#include "../../Property/Properties/PickuppableProperty.h"
#include "../../Property/Properties/WaterContainerProperty.h"

WaterskinEntity::WaterskinEntity() : Entity("", "Waterskin", "$[brown]$(male)") {
    addProperty(std::make_unique<WaterContainerProperty>());
    addProperty(std::make_unique<PickuppableProperty>(1));
}