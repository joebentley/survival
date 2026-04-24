#pragma once

#include "../../Entity/Entity.h"
#include "../../Font.h"

void drawDescriptionScreen(Font &font, Entity &item);

enum class ScreenType { NOTIFICATION, INVENTORY, LOOTING, INSPECTION, CRAFTING, EQUIPMENT, HELP, DEBUG };

struct Screen {
    explicit Screen(bool shouldRenderWorld) : mShouldRenderWorld(shouldRenderWorld) {}

    virtual ~Screen() = default;

    virtual void enable() { mEnabled = true; }
    virtual void disable() { mEnabled = false; }
    bool isEnabled() const { return mEnabled; }
    bool shouldRenderWorld() const { return mShouldRenderWorld; }
    virtual void handleInput(SDL_KeyboardEvent &e) = 0;
    virtual void render(Font &font) = 0;

  protected:
    bool mEnabled{false};
    bool mShouldRenderWorld;
};
