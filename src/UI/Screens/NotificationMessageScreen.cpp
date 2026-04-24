#include "NotificationMessageScreen.h"

#include "../NotificationMessageRenderer.h"

void NotificationMessageScreen::handleInput(SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_ESCAPE:
    case SDLK_M:
        mEnabled = false;
        break;
    }
}

void NotificationMessageScreen::render(Font &font) {
    auto messages = NotificationMessageRenderer::getInstance().getMessages();

    const int numMessagesToShow = World::SCREEN_HEIGHT - 4;
    const int xOffset = 4;

    font.drawText("Message history", xOffset, 1);
    font.drawText("m or esc to exit", World::SCREEN_WIDTH - 20, 1);

    auto front = messages.crbegin();
    for (int i = 0; front != messages.crend() && i < numMessagesToShow; ++i, ++front) {
        font.drawText(*front, xOffset, numMessagesToShow + 1 - i);
    }
}
