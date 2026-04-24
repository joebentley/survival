#pragma once

#include "Screen.h"

struct NotificationMessageScreen : Screen {
    NotificationMessageScreen() : Screen(false) {}

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font &font) override;
};
