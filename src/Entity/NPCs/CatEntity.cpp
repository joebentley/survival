#include "CatEntity.h"
#include "../Items/Food/CorpseEntity.h"

void CatEntity::destroy() {
    auto corpse = std::make_unique<CorpseEntity>(mID + "corpse", 0.4, mName, 100);
    corpse->setPos(getPos());
    EntityManager::getInstance().addEntity(std::move(corpse));
}