#include "Entities.h"
#include "World.h"

#include <algorithm>
#include <any>

bool PlayerEntity::attack(const Point &attackPos) {
    auto entitiesInSquare = EntityManager::getInstance().getEntitiesAtPosFaster(attackPos);

    if (entitiesInSquare.empty()) {
        return false;
    }

    auto enemy = entitiesInSquare[0];
    // roll and damage the enemy
    int damage = rollDamage();
    enemy->mHp -= damage;

    // send hit notification message
    NotificationMessageRenderer::getInstance().queueMessage(
            "$(dwarf) hit " + enemy->mGraphic + " " + enemy->mName + "$[white] with $[red]$(heart)$[white]" + std::to_string(mHitTimes) +
            "d" + std::to_string(mHitAmount) + " for " + std::to_string(damage));

    // TODO: Add AV
    // TODO: Add avoidance
    // force enemy to start attacking player
    if (enemy->getBehaviourByID("WanderBehaviour") != nullptr)
        enemy->getBehaviourByID("WanderBehaviour")->disable();
    if (enemy->getBehaviourByID("AttachmentBehaviour") != nullptr)
        enemy->getBehaviourByID("AttachmentBehaviour")->disable();
    if (enemy->getBehaviourByID("WanderAttachBehaviour") != nullptr)
        enemy->getBehaviourByID("WanderAttachBehaviour")->disable();
    if (enemy->getBehaviourByID("ChaseAndAttackBehaviour") != nullptr)
        enemy->getBehaviourByID("ChaseAndAttackBehaviour")->enable();

    // set the player's attack target to the enemy
    auto& ui = dynamic_cast<StatusUIEntity&>(*EntityManager::getInstance().getEntityByID("StatusUI"));
    ui.setAttackTarget(enemy->mID);

    // if the enemy died, delete the enemy and stop attacking
    if (enemy->mHp <= 0) {
        ui.clearAttackTarget();
        EntityManager::getInstance().queueForDeletion(enemy->mID);
        attacking = false;
        NotificationMessageRenderer::getInstance().queueMessage(enemy->mGraphic + " " + enemy->mName + "$[white] was ${black}$[red]destroyed!");
        return false;
    }

    attacking = true;
    return true;
}

void PlayerEntity::tick() {
    hunger -= hungerRate;

    if (hunger < 0.3)
        mHp -= hungerDamageRate;

    Entity::tick();
}

void PlayerEntity::handleInput(SDL_KeyboardEvent &e, bool &quit, std::unordered_map<ScreenType, std::unique_ptr<Screen>> &screens)
{
    auto key = e.keysym.sym;
    auto mod = e.keysym.mod;

    bool didAction = false;

    if (showingTooMuchWeightMessage) {
        if (key == SDLK_ESCAPE || key == SDLK_RETURN)
            showingTooMuchWeightMessage = false;
        return;
    }

    // Keep interacting with the entity, bypass other interactions
    if (interactingWithEntity) {
        auto entity = EntityManager::getInstance().getEntityByID(mEntityInteractingWith);
        auto b = entity->getBehaviourByID("InteractableBehaviour");
        if (!dynamic_cast<InteractableBehaviour&>(*b).handleInput(e)) {
            mEntityInteractingWith.clear();
            interactingWithEntity = false;
        }
        return;
    }

    if (mHp > 0) {
        // Handle interaction
        if (key == SDLK_SPACE) {
            auto entitiesSurrounding = EntityManager::getInstance().getEntitiesSurroundingFaster(mPos);

            // Just use the first interactable entity found
            for (auto &entity : entitiesSurrounding) {
                auto b = entity->getBehaviourByID("InteractableBehaviour");

                if (b != nullptr) {
                    interactingWithEntity = true;
                    mEntityInteractingWith = entity->mID;

                    // perform an initial interaction
                    if (!dynamic_cast<InteractableBehaviour&>(*b).handleInput(e)) {
                        mEntityInteractingWith.clear();
                        interactingWithEntity = false;
                    }
                }
            }
        }

        // Handle looting
        if (key == SDLK_g) {
            auto entitiesAtPos = EntityManager::getInstance().getEntitiesAtPosFaster(mPos);

            // TODO: need to handle multiple items on same square properly
//            std::vector<std::shared_ptr<Entity>> waterEntities;

            std::vector<Entity *> entitiesWithInventories;
            std::copy_if(entitiesAtPos.begin(), entitiesAtPos.end(), std::back_inserter(entitiesWithInventories),
                         [](auto &a) {
                return a->mID != "Player" && !a->isInventoryEmpty();
            });

            // TODO: chests are broken right now (no way to open)!

            if (!entitiesWithInventories.empty()) {
                // TODO: what if multiple chests are present?
                auto entity = entitiesWithInventories[0];
                if (entity->mSkipLootingDialog) {
                    // Loot just the first item
                    if (addToInventory(entity->getInventoryItem(0)->mID)) {
                        entity->removeFromInventory(0);
                        didAction = true;
                        goto postLooting;
                    } else {
                        showingTooMuchWeightMessage = true;
                        return;
                    }
                } else {
                    auto &lootingDialog = dynamic_cast<LootingDialog&>(*screens[ScreenType::LOOTING]);
                    lootingDialog.showItemsToLoot(entity->getInventory(), entity);
                    return;
                }
            }

            std::vector<Entity *> pickuppableEntities;
            std::copy_if(entitiesAtPos.begin(), entitiesAtPos.end(), std::back_inserter(pickuppableEntities),
                         [this](auto &a) {
                auto b = a->hasProperty("Pickuppable");
                // Don't pick up if it isn't pickuppable, or if it is already in the player's inventory
                return (b && !isInInventory(a->mID));
            });

            if (pickuppableEntities.empty())
                return;
            else if (pickuppableEntities.size() == 1) {
                if (addToInventory((*pickuppableEntities.begin())->mID)) {
                    didAction = true;
                    goto postLooting;
                } else {
                    showingTooMuchWeightMessage = true;
                    return;
                }
            } else {
                auto &lootingDialog = dynamic_cast<LootingDialog&>(*screens[ScreenType::LOOTING]);
                lootingDialog.showItemsToLoot(pickuppableEntities);
                return;
            }
        }

        postLooting:

        if (mHp > 0 && (key == SDLK_h || key == SDLK_j || key == SDLK_k || key == SDLK_l ||
                       key == SDLK_y || key == SDLK_u || key == SDLK_b || key == SDLK_n)) // Only ever attack if moving
        {
            Point posOffset;
            switch (key) {
                case SDLK_h:
                    posOffset = Point(-1, 0);
                    break;
                case SDLK_j:
                    posOffset = Point(0, 1);
                    break;
                case SDLK_k:
                    posOffset = Point(0, -1);
                    break;
                case SDLK_l:
                    posOffset = Point(1, 0);
                    break;
                case SDLK_y:
                    posOffset = Point(-1, -1);
                    break;
                case SDLK_u:
                    posOffset = Point(1, -1);
                    break;
                case SDLK_b:
                    posOffset = Point(-1, 1);
                    break;
                case SDLK_n:
                    posOffset = Point(1, 1);
                    break;
                default:
                    posOffset = Point(0, 0);
                    break;
            }

            Point newPos = getPos() + posOffset;

            std::vector<Entity*> entitiesInSpace = EntityManager::getInstance().getEntitiesAtPosFaster(newPos);

            if (!entitiesInSpace.empty()) {
                // TODO: what if more than one enemy in space?
                Entity *entity = entitiesInSpace[0];

                // Check whether or not to attack
                if (
                        entity->canBeAttacked()
                            && (
                                    (entity->getBehaviourByID("HostilityBehaviour") != nullptr && entity->getBehaviourByID("HostilityBehaviour")->isEnabled())
                                 || (mod & KMOD_SHIFT) // force attack // NOLINT(hicpp-signed-bitwise)
                                 || attacking // already attacking
                               )
                   )
                {
                    attack(newPos);
                    didAction = true;
                }
            }

            // Try to move into the new space if we didn't attack
            if (!didAction && !moveTo(newPos))
                return; // Don't tick if didn't move

            didAction = true;
        }

        if (key == SDLK_i) {
            screens[ScreenType::INVENTORY]->enable();
        }

        if (key == SDLK_PERIOD) {
            EntityManager::getInstance().broadcast(SIGNAL_FORCE_WAIT);
            didAction = true;
        }

        if (key == SDLK_SEMICOLON) {
            auto &b = dynamic_cast<InspectionDialog&>(*screens[ScreenType::INSPECTION]);
            b.enableAtPoint(mPos);
        }

        if (key == SDLK_c)
            screens[ScreenType::CRAFTING]->enable();

        if (key == SDLK_e)
            screens[ScreenType::EQUIPMENT]->enable();

        if (key == SDLK_m)
            screens[ScreenType::NOTIFICATION]->enable();

        if (mod & KMOD_SHIFT && key == SDLK_SLASH) // NOLINT(hicpp-signed-bitwise)
            screens[ScreenType::HELP]->enable();

        if (mod & KMOD_SHIFT && key == SDLK_d) // NOLINT(hicpp-signed-bitwise)
            screens[ScreenType::DEBUG]->enable();
    }

    if (didAction)
        EntityManager::getInstance().tick();

    if (mHp <= 0 && key == SDLK_RETURN)
        quit = true;
}

void PlayerEntity::render(Font &font, Point currentWorldPos) {
    Entity::render(font, currentWorldPos);

    if (showingTooMuchWeightMessage) {
        const std::string& displayString = "You cannot carry that much!";
        MessageBoxRenderer::getInstance().queueMessageBoxCentered(displayString, 1);
    }

    if (interactingWithEntity) {
        auto entityInteractingWith = EntityManager::getInstance().getEntityByID(mEntityInteractingWith);
        dynamic_cast<InteractableBehaviour&>(*entityInteractingWith->getBehaviourByID("InteractableBehaviour")).render(font);
    }
}

bool PlayerEntity::addToInventory(const std::string &ID) {
    if (Entity::addToInventory(ID)) {
        auto entity = EntityManager::getInstance().getEntityByID(ID);
        NotificationMessageRenderer::getInstance()
                .queueMessage("You got a " + entity->mGraphic + " " + entity->mName + "${transparent}$[white]!");
        return true;
    }
    return false;
}

void PlayerEntity::addHunger(float hungerRestoration) {
    hunger = std::min(hunger + hungerRestoration, 1.0f);
}

void StatusUIEntity::render(Font &font, Point /*currentWorldPos*/) {
    std::string colorStr;
    float hpPercent = player.mHp / player.mMaxHp;

    if (hpPercent > 0.7)
        colorStr = "$[green]";
    else if (hpPercent > 0.3)
        colorStr = "$[yellow]";
    else
        colorStr = "$[red]";

    font.drawText("${black}" + colorStr + "hp " + std::to_string((int)ceil(player.mHp))
                  + "/" + std::to_string((int)ceil(player.mMaxHp)), SCREEN_WIDTH - X_OFFSET, 1);

    if (player.hunger > 0.7)
        font.drawText("${black}$[green]sated", SCREEN_WIDTH - X_OFFSET, 2);
    else if (player.hunger > 0.3)
        font.drawText("${black}$[yellow]hungry", SCREEN_WIDTH - X_OFFSET, 2);
    else
        font.drawText("${black}$[red]starving", SCREEN_WIDTH - X_OFFSET, 2);

    font.drawText("$[red]$(heart)$[white]" + std::to_string(player.mHitTimes) + "d" + std::to_string(player.computeMaxDamage())
            , SCREEN_WIDTH - X_OFFSET, 3);

    font.drawText("${black}" + std::to_string(player.getCarryingWeight()) + "/" + std::to_string(player.getMaxCarryWeight()) + "lb",
                  SCREEN_WIDTH - X_OFFSET, 4);

    if (forceTickDisplayTimer-- > 0) {
        font.drawText("Waited " + std::to_string(ticksWaitedDuringAnimation)
                      + " tick" + (ticksWaitedDuringAnimation > 1 ? "s" : "") + "...",
                      SCREEN_WIDTH - X_OFFSET - 8, SCREEN_HEIGHT - 2,
                      Color(0xFF, 0xFF, 0xFF, static_cast<Uint8>(static_cast<float>(forceTickDisplayTimer) / FORCE_TICK_DISPLAY_LENGTH * 0xFF)),
                FontColor::getColor("transparent"));
    } else {
        forceTickDisplayTimer = 0;
        ticksWaitedDuringAnimation = 1;
    }

    if (!mAttackTargetID.empty()) {
        auto attackTarget = EntityManager::getInstance().getEntityByID(mAttackTargetID);
        float enemyhpPercent = attackTarget->mHp / attackTarget->mMaxHp;
        std::string enemyhpString = "${black}";

        if (enemyhpPercent == 1)
            enemyhpString += "$[white]unscathed";
        else if (enemyhpPercent > 0.7)
            enemyhpString += "$[green]moderate";
        else if (enemyhpPercent > 0.3)
            enemyhpString += "$[yellow]badly wounded";
        else
            enemyhpString += "$[red]near death";

        font.drawText(enemyhpString, SCREEN_WIDTH - X_OFFSET - 2, 6);
    }

    // Drop attack target after 10 turns of inactivity
    if (attackTargetTimer == 0)
        mAttackTargetID.clear();

    font.drawText(EntityManager::getInstance().getTimeOfDay().toWordString(), SCREEN_WIDTH - X_OFFSET, 8);
}

void StatusUIEntity::emit(uint32_t signal) {
    if (signal & SIGNAL_FORCE_WAIT) {
        if (forceTickDisplayTimer > 0) {
            ticksWaitedDuringAnimation++;
        }
        forceTickDisplayTimer = FORCE_TICK_DISPLAY_LENGTH;
    }

    Entity::emit(signal);
}

void StatusUIEntity::tick() {
    if (!mAttackTargetID.empty()) {
        auto attackTarget = EntityManager::getInstance().getEntityByID(mAttackTargetID);
        if (attackTarget->getBehaviourByID("ChaseAndAttackBehaviour") != nullptr &&
            !attackTarget->getBehaviourByID("ChaseAndAttackBehaviour")->isEnabled())
        {
            attackTargetTimer--;
        }
    }

    Entity::tick();
}

void CatEntity::destroy() {
    auto corpse = std::make_unique<CorpseEntity>(mID + "corpse", 0.4, mName, 100);
    corpse->setPos(getPos());
    EntityManager::getInstance().addEntity(std::move(corpse));
}

void WolfEntity::destroy() {
    auto corpse = std::make_unique<CorpseEntity>(mID + "corpse", 0.4, mName, 100);
    corpse->setPos(getPos());
    EntityManager::getInstance().addEntity(std::move(corpse));
}

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

void FireEntity::render(Font &font, Point currentWorldPos) {
    if (fireLevel < 0.1)
        mGraphic = "${black}$[grey]%";
    else if (rand() % 2 == 0)
        mGraphic = "${black}$[red]%";
    else
        mGraphic = "${black}$[orange]%";

    std::any_cast<LightEmittingProperty::Light>(getProperty("LightEmitting")->getValue())
            .setRadius(static_cast<int>(std::round(6 * fireLevel)));

    Entity::render(font, currentWorldPos);
}

void FireEntity::tick() {
    fireLevel -= 0.005f;
}

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

bool FireEntity::RekindleBehaviour::handleInput(SDL_KeyboardEvent &e) {
    switch (e.keysym.sym) {
        case SDLK_j:
            if (choosingItemToUse) {
                const auto &player = EntityManager::getInstance().getEntityByID("Player");
                const auto &entities = player->filterInventoryForCraftingMaterials(std::vector<std::string> {"grass", "wood"});

                if ((size_t)choosingItemIndex == entities.size() - 1)
                    choosingItemIndex = 0;
                else
                    ++choosingItemIndex;
            }
            break;
        case SDLK_k:
            if (choosingItemToUse) {
                const auto &player = EntityManager::getInstance().getEntityByID("Player");
                const auto &entities = player->filterInventoryForCraftingMaterials(std::vector<std::string> {"grass", "wood"});

                if (choosingItemIndex == 0)
                    choosingItemIndex = static_cast<int>(entities.size()) - 1;
                else
                    ++choosingItemIndex;
            }
            break;
        case SDLK_RETURN:
            if (!choosingItemToUse) {
                choosingItemToUse = true;
                break;
            } else {
                const auto &player = EntityManager::getInstance().getEntityByID("Player");
                const auto &entities = player->filterInventoryForCraftingMaterials(std::vector<std::string> {"grass", "wood"});
                player->removeFromInventory(entities[choosingItemIndex]);
                dynamic_cast<FireEntity&>(mParent).fireLevel = 1;
                NotificationMessageRenderer::getInstance().queueMessage("$[red]Rekindled fire");
                choosingItemToUse = false;
                return false;
            }
        case SDLK_ESCAPE:
            if (choosingItemToUse) {
                choosingItemToUse = false;
                choosingItemIndex = 0;
                break;
            } else
                return false;
    }

    return true;
}

void FireEntity::RekindleBehaviour::render(Font &/*font*/) {
    MessageBoxRenderer::getInstance().queueMessageBoxCentered("$(right)Rekindle", 1);

    if (choosingItemToUse) {
        const auto &player = EntityManager::getInstance().getEntityByID("Player");
        const auto &entities = player->filterInventoryForCraftingMaterials(std::vector<std::string> {"grass", "wood"});

        if (entities.empty()) {
            choosingItemToUse = false;
            return;
        }

        std::vector<std::string> displayStrings;

        for (std::vector<std::string>::size_type i = 0; i < entities.size(); ++i) {
            const auto &entity = EntityManager::getInstance().getEntityByID(entities[i]);
            displayStrings.emplace_back((i == (size_t)choosingItemIndex ? "$(right)" : " ") + entity->mGraphic + " " + entity->mName);
        }

        MessageBoxRenderer::getInstance().queueMessageBoxCentered(displayStrings, 1);
    }
}

void GlowbugEntity::render(Font &font, Point currentWorldPos) {
    static int timer = 0;

    if (timer++ > (rand() % 20) + 20){
        switch (rand() % 3) {
            case 0:
                mGraphic = "$[green]`";
                break;
            case 1:
                mGraphic = "$[green]'";
                break;
            case 2:
                mGraphic = "";
                break;
            default:
                break;
        }

        timer = 0;
    }

    Entity::render(font, currentWorldPos);
}

void BunnyEntity::render(Font &font, Point currentWorldPos) {
    static int i = 0;
    i++;

    mGraphic = i > 30 ? "$(bunny1)" : "$(bunny2)";

    if (i > 60)
        i = 0;

    Entity::render(font, currentWorldPos);
};

BuildingWallEntity::BuildingWallEntity(const Point &pos, const std::vector<std::string> &layout) : Entity("", "Wall", "") {
    this->setPos(pos);

    // generate the walls from the given layout string
    int x = 0;
    int y = 0;

    std::unordered_set<Point> walls;

    for (const std::string &line : layout) {
        for (char character : line) {
            if (character == '+') {
                walls.insert(Point(x, y));
            } else if (character == 'd') {
                auto door = std::make_unique<DoorEntity>(pos + Point(x, y));
                EntityManager::getInstance().addEntity(std::move(door));
            }

            x++;
        }
        x = 0;
        y++;
    }

    // generate the correct wall tile types
    generateWallsFromPoints(walls);

    // render on top
    mRenderingLayer = -1;

    // should not be able to attack the wall
    mCanBeAttacked = true;
}

/// To neatly check if a set of Points has a given Point. There's probably a better way but this works fine
struct SetHas {
    explicit SetHas(std::unordered_set<Point> points) : points(std::move(points)) {}

    bool has(const Point &pos) {
        return points.find(pos) != points.cend();
    }

    std::unordered_set<Point> points;
};

void BuildingWallEntity::generateWallsFromPoints(const std::unordered_set<Point> &points) {
    auto h = SetHas(points);

    // Work out the neighbour points for each point, and choose the correct WallType correspondingly
    for (const Point &p : points) {
        if (h.has(p + Point(0, -1)) && h.has(p + Point(-1, 0)) && h.has(p + Point(1, 0)) && h.has(p + Point(0, 1)))
            mWalls.insert({p, WallType::CROSS});
        else if (h.has(p + Point(0, -1)) && h.has(p + Point(-1, 0)) && h.has(p + Point(0, 1)))
            mWalls.insert({p, WallType::T_LEFT});
        else if (h.has(p + Point(0, -1)) && h.has(p + Point(-1, 0)) && h.has(p + Point(1, 0)))
            mWalls.insert({p, WallType::T_UP});
        else if (h.has(p + Point(0, -1)) && h.has(p + Point(1, 0)) && h.has(p + Point(0, 1)))
            mWalls.insert({p, WallType::T_RIGHT});
        else if (h.has(p + Point(1, 0)) && h.has(p + Point(-1, 0)) && h.has(p + Point(0, 1)))
            mWalls.insert({p, WallType::T_DOWN});
        else if (h.has(p + Point(0, -1)) && h.has(p + Point(-1, 0)))
            mWalls.insert({p, WallType::UL_CORNER});
        else if (h.has(p + Point(0, -1)) && h.has(p + Point(1, 0)))
            mWalls.insert({p, WallType::UR_CORNER});
        else if (h.has(p + Point(0, 1)) && h.has(p + Point(-1, 0)))
            mWalls.insert({p, WallType::DL_CORNER});
        else if (h.has(p + Point(0, 1)) && h.has(p + Point(1, 0)))
            mWalls.insert({p, WallType::DR_CORNER});
        else if (h.has(p + Point(0, -1)) && h.has(p + Point(0, 1)))
            mWalls.insert({p, WallType::VERT});
        else if (h.has(p + Point(-1, 0)) && h.has(p + Point(1, 0)))
            mWalls.insert({p, WallType::HORIZ});
        else if (h.has(p + Point(0, -1)) || h.has(p + Point(0, 1)))
            mWalls.insert({p, WallType::VERT});
        else if (h.has(p + Point(-1, 0)) || h.has(p + Point(1, 0)))
            mWalls.insert({p, WallType::HORIZ});
        else
            mWalls.insert({p, WallType::CROSS}); // this should just be lonely wall tiles
    }
}

void BuildingWallEntity::render(Font& font, Point currentWorldPos) {
    // Only draw if the entity is on the current world screen
    if (isOnScreen(currentWorldPos)) {
        // draw each wall tile
        for (const auto &pair : mWalls) {
            Point wallPos = pair.first;
            Point screenPos = worldToScreen(mPos + wallPos);
            WallType wallType = pair.second;

            std::string c; // the character we will draw

            // choose the correct fontstring character for each wall type. Here we choose the double-thickness pipe walls
            switch (wallType) {
                case WallType::CROSS:
                    c = "p28";
                    break;
                case WallType::T_LEFT:
                    c = "p7";
                    break;
                case WallType::T_RIGHT:
                    c = "p26";
                    break;
                case WallType::T_UP:
                    c = "p24";
                    break;
                case WallType::T_DOWN:
                    c = "p25";
                    break;
                case WallType::UL_CORNER:
                    c = "p10";
                    break;
                case WallType::UR_CORNER:
                    c = "p22";
                    break;
                case WallType::DL_CORNER:
                    c = "p9";
                    break;
                case WallType::DR_CORNER:
                    c = "p23";
                    break;
                case WallType::VERT:
                    c = "p8";
                    break;
                case WallType::HORIZ:
                    c = "p27";
                    break;
            }

            font.draw(c, screenPos, FontColor::getColor("white"), FontColor::getColor("black"));
        }
    }
}

bool BuildingWallEntity::collide(const Point &pos) {
    // the grid position on our BuildingWallEntity's "blueprints" (i.e. mWalls)
    Point wallPos = pos - mPos;
    // check if this point is in mWalls
    return mWalls.find(wallPos) != mWalls.cend();
}

DoorEntity::DoorEntity(const Point &pos) : Entity("", "Door", "") {
    mPos = pos;
    mRenderingLayer = 0;

    auto interactable = std::make_unique<DoorOpenAndCloseBehaviour>(*this);
    addBehaviour(std::move(interactable));
}

bool DoorEntity::DoorOpenAndCloseBehaviour::handleInput(SDL_KeyboardEvent &) {
    std::string message;
    auto &parent = dynamic_cast<DoorEntity&>(mParent);

    if (parent.isOpen()) {
        message = "You closed the door";
        parent.close();
    } else {
        message = "You opened the door";
        parent.open();
    }

    NotificationMessageRenderer::getInstance().queueMessage(message);

    return false;
}


void DoorEntity::render(Font& font, Point currentWorldPos) {
    // Change graphic depending on whether door open or closed
    mGraphic = std::string("$[white]${black}") + std::string(mIsOpen ? "." : "+");
    Entity::render(font, currentWorldPos);
}

bool DoorEntity::collide(const Point &pos) {
    // Open the door if we walk into it and it's currently shut
    if (!mIsOpen && pos == mPos) {
        this->open();
        return true;
    }
    return false;
}