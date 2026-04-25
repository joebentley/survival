#include "WaterContainerProperty.h"

WaterContainerProperty::WaterContainerProperty(int maxCapacity) : mMaxCapacity(maxCapacity) {}

[[nodiscard]] int WaterContainerProperty::getMaxCapacity() const { return mMaxCapacity; }
void WaterContainerProperty::setMaxCapacity(int maxCapacity) { mMaxCapacity = maxCapacity; }

[[nodiscard]] int WaterContainerProperty::getAmount() const { return mCurrentAmount; }
void WaterContainerProperty::setAmount(int currentAmount) { mCurrentAmount = currentAmount; }
void WaterContainerProperty::addAmount(int amount) { mCurrentAmount = std::min(mMaxCapacity, mCurrentAmount + amount); }

PROPERTY_SUBCLASS_TYPE_STRING(WaterContainer)
