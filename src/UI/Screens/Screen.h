#pragma once

#include <SDL3/SDL_events.h>

class Font;
struct Entity;

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
