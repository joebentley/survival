#include "Game.h"

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void **appstate_void, int argc, char *argv[]) {
    auto *appstate = new Game();
    *appstate_void = appstate;

    SDL_Log("Reached end of init!");

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate_void, SDL_Event *event) {
    Game *appState = static_cast<Game *>(appstate_void);

    if (appState->processEvent(event)) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate_void) {
    Game *appState = static_cast<Game *>(appstate_void);
    appState->iterate();

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate_void, SDL_AppResult result) { delete static_cast<Game *>(appstate_void); }
