#pragma once

#include "../PlayerEntity.h"

class StatusUIEntity : public Entity {
    const int FORCE_TICK_DISPLAY_LENGTH{100};

    PlayerEntity &player;

    int forceTickDisplayTimer{0};
    int ticksWaitedDuringAnimation{1};
    int attackTargetTimer{0};

    std::string mAttackTargetID;
    const int X_OFFSET = 10;

  public:
    StatusUIEntity()
        : StatusUIEntity(dynamic_cast<PlayerEntity &>(*EntityManager::getInstance().getEntityByID("Player"))) {}

    explicit StatusUIEntity(PlayerEntity &player) : Entity("StatusUI", "", ""), player(player) {
        mRenderingLayer = 1; // Keep on background
        mCanBeAttacked = false;
    }

    void render(Font &font, Point currentWorldPos) override;
    void emit(Uint32 signal) override;
    void tick() override;
    void setAttackTarget(const std::string &attackTargetID) {
        mAttackTargetID = attackTargetID;
        attackTargetTimer = 10;
    }
    void clearAttackTarget() { mAttackTargetID.clear(); }
    //    void showLootedItemNotification(std::string itemString);
};
