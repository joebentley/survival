#include "BushEntity.h"

#include "../../Behaviour/Item/KeepStockedBehaviour.h"
#include "../Items/Food/BerryEntity.h"

void BushEntity::render(Font &font, Point currentWorldPos) {
    if (!isInventoryEmpty()) {
        mGraphic = "${black}$[purple]$(div)";
    } else {
        mGraphic = "${black}$[green]$(div)";
    }

    Entity::render(font, currentWorldPos);
}

BushEntity::BushEntity(std::string ID) : Entity(std::move(ID), "Bush", "${black}$[purple]$(div)") {
    mShortDesc = SHORT_DESC;
    mLongDesc = LONG_DESC;
    mSkipLootingDialog = true;
    addBehaviour(std::make_unique<KeepStockedBehaviour<BerryEntity>>(*this, RESTOCK_RATE));
    auto item = std::make_unique<BerryEntity>();
    auto itemID = item->mID;
    EntityManager::getInstance().addEntity(std::move(item));
    addToInventory(itemID);
}
