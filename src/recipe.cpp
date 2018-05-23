#include "recipe.h"

RecipeManager::RecipeManager() {
    mRecipes.emplace_back(std::make_shared<FireRecipe>());
    mRecipes.emplace_back(std::make_shared<BandageRecipe>());
    mRecipes.emplace_back(std::make_shared<TorchRecipe>());
    mRecipes.emplace_back(std::make_shared<BagRecipe>());
}

int FireRecipe::numProduced = 0;
void FireRecipe::produce() {
    auto &em = EntityManager::getInstance();
    auto fire = std::make_unique<FireEntity>("fire" + std::to_string(++numProduced));
    fire->setPos(mPointIfNotGoingIntoInventory);
    em.addEntity(std::move(fire));
}

int BandageRecipe::numProduced = 0;
void BandageRecipe::produce() {
    auto &em = EntityManager::getInstance();
    auto player = em.getEntityByID("Player");
    auto bandage = std::make_unique<BandageEntity>("bandage" + std::to_string(++numProduced));
    auto ID = bandage->mID;
    EntityManager::getInstance().addEntity(std::move(bandage));
    player->addToInventory(ID);
}

int TorchRecipe::numProduced = 0;
void TorchRecipe::produce() {
    auto &em = EntityManager::getInstance();
    auto player = em.getEntityByID("Player");
    auto torch = std::make_unique<TorchEntity>("torch" + std::to_string(++numProduced));
    auto ID = torch->mID;
    EntityManager::getInstance().addEntity(std::move(torch));
    player->addToInventory(ID);
}

void BagRecipe::produce() {
    auto &em = EntityManager::getInstance();
    auto player = em.getEntityByID("Player");
    auto bag = std::make_unique<BagEntity>();
    auto ID = bag->mID;
    EntityManager::getInstance().addEntity(std::move(bag));
    player->addToInventory(ID);
}
