#include "SDLManager.h"
#include <iostream>

int SDLManager::initialize(Uint32 SDLInitFlags, Uint32 SDLImageInitFlags) {
    if (SDL_Init(SDLInitFlags) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    } else if (IMG_Init(SDLImageInitFlags) != SDLImageInitFlags) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return -1;
    } else {
        mWindow = SDL_CreateWindow("Survival",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

        if (mWindow == nullptr) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return -1;
        } else {
            mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);

            if (mRenderer == nullptr) {
                std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
                return -1;
            }
        }
    }

    return 0;
}

SDL_Window * SDLManager::getWindow() const {
    return mWindow;
}

SDL_Renderer * SDLManager::getRenderer() const {
    return mRenderer;
}

SDLManager::~SDLManager() {
    if (mRenderer != nullptr)
        SDL_DestroyRenderer(mRenderer);
    if (mWindow != nullptr)
        SDL_DestroyWindow(mWindow);

    SDL_Quit();
}
