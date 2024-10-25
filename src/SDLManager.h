#ifndef SURVIVAL_WINDOW_H
#define SURVIVAL_WINDOW_H

//#define XCODE 1

#ifdef _MSC_VER
#include <SDL_image.h>
#include <SDL.h>
#elif XCODE
#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL.h>
#else
#include <SDL_image.h>
#include <SDL2/SDL.h>
#endif

#include <memory>
#include "Font.h"
#include "World.h"

const int WINDOW_WIDTH = CHAR_WIDTH * SCREEN_WIDTH;
const int WINDOW_HEIGHT = CHAR_HEIGHT * SCREEN_HEIGHT;

class SDLManager {
public:
    ~SDLManager();

    int initialize(Uint32 SDLInitFlags, Uint32 SDLImageInitFlags);

    SDL_Window *getWindow() const;
    SDL_Renderer *getRenderer() const;
private:
    SDL_Window *mWindow {nullptr};
    SDL_Renderer *mRenderer {nullptr};
};

#endif //SURVIVAL_WINDOW_H
