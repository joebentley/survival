#include "SDLManager.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>

SDL_Window *SDLManager::getWindow() const { return mWindow; }

SDL_Renderer *SDLManager::getRenderer() const { return mRenderer; }

SDLManager::SDLManager(Uint32 SDLInitFlags) {
    if (!SDL_Init(SDLInitFlags)) {
        SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
    }

    mWindow = SDL_CreateWindow("Survival", WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if (mWindow == nullptr) {
        SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
    }

    mRenderer = SDL_CreateRenderer(mWindow, nullptr);

    if (mRenderer == nullptr) {
        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
    }
}

SDLManager::~SDLManager() {
    if (mRenderer != nullptr)
        SDL_DestroyRenderer(mRenderer);
    if (mWindow != nullptr)
        SDL_DestroyWindow(mWindow);

    SDL_Quit();
}
