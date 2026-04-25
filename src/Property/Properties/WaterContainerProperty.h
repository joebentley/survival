#pragma once

#include "../Property.h"

class WaterContainerProperty : public Property {
  public:
    PROPERTY_SUBCLASS_BODY(WaterContainer)

    explicit WaterContainerProperty(int maxCapacity = 64);

    [[nodiscard]] int getMaxCapacity() const;
    void setMaxCapacity(int maxCapacity);

    [[nodiscard]] int getAmount() const;
    void setAmount(int currentAmount);
    void addAmount(int amount);

  private:
    int mMaxCapacity{0};
    int mCurrentAmount{0};
};
