#include "SDLManager.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>

int SDLManager::initialize(Uint32 SDLInitFlags) {
    if (!SDL_Init(SDLInitFlags)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    } else {
        mWindow = SDL_CreateWindow("Survival", WINDOW_WIDTH, WINDOW_HEIGHT, 0);

        if (mWindow == nullptr) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return -1;
        } else {
            // SDL_RENDERER_HARDWARE has become slow when using SDL_RenderFillRect
            mRenderer = SDL_CreateRenderer(mWindow, nullptr);

            if (mRenderer == nullptr) {
                std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
                return -1;
            }
        }
    }

    return 0;
}

SDL_Window *SDLManager::getWindow() const { return mWindow; }

SDL_Renderer *SDLManager::getRenderer() const { return mRenderer; }

SDLManager::~SDLManager() {
    if (mRenderer != nullptr)
        SDL_DestroyRenderer(mRenderer);
    if (mWindow != nullptr)
        SDL_DestroyWindow(mWindow);

    SDL_Quit();
}
