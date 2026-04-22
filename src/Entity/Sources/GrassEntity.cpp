#include "GrassEntity.h"
#include "../../Behaviours.h"
#include "../Items/Materials/GrassTuftEntity.h"

void GrassEntity::render(Font &font, Point currentWorldPos) {
    if (!isInventoryEmpty()) {
        mGraphic = "${black}$[grasshay]$(tau)";
    } else {
        mGraphic = "${black}$[grasshay].";
    }

    Entity::render(font, currentWorldPos);
}

GrassEntity::GrassEntity(std::string ID) : Entity(std::move(ID), "Grass", "${black}$[grasshay]$(tau)") {
    mShortDesc = SHORT_DESC;
    mLongDesc = LONG_DESC;
    mSkipLootingDialog = true;
    addBehaviour(std::make_unique<KeepStockedBehaviour<GrassTuftEntity>>(*this, RESTOCK_RATE));
    auto item = std::make_unique<GrassTuftEntity>();
    auto itemID = item->mID;
    EntityManager::getInstance().addEntity(std::move(item));
    addToInventory(itemID);
}
