#include "HelpScreen.h"
#include "../../Font.h"

void HelpScreen::handleInput(SDL_KeyboardEvent &e) {
    if (e.key == SDLK_ESCAPE || (e.key == SDLK_SLASH && e.mod & SDL_KMOD_SHIFT))
        disable();
}

void HelpScreen::render(Font &font) {
    const int yPadding = 2;
    const int xPadding = 2;
    int y = 0;

    for (const auto &line : cDisplayLines) {
        font.drawText(line, xPadding, yPadding + y++);
    }
}
