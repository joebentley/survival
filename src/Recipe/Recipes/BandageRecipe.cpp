#include "BandageRecipe.h"

#include "../../Entity/EntityManager.h"
#include "../../Entity/Items/Healing/BandageEntity.h"

BandageRecipe::BandageRecipe() : Recipe("Bandage") { mIngredients.emplace_back(Ingredient{1, "grass"}); }

int BandageRecipe::numProduced = 0;
void BandageRecipe::produce() {
    auto &em = EntityManager::getInstance();
    auto player = em.getEntityByID("Player");
    auto bandage = std::make_unique<BandageEntity>("bandage" + std::to_string(++numProduced));
    auto ID = bandage->mID;
    EntityManager::getInstance().addEntity(std::move(bandage));
    player->addToInventory(ID);
}
