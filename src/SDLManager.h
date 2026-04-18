#ifndef SURVIVAL_WINDOW_H
#define SURVIVAL_WINDOW_H

#include <SDL3/SDL.h>

#include "Font.h"
#include "World.h"
#include <memory>

const int WINDOW_WIDTH = CHAR_WIDTH * SCREEN_WIDTH;
const int WINDOW_HEIGHT = CHAR_HEIGHT * SCREEN_HEIGHT;

class SDLManager {
  public:
    ~SDLManager();

    int initialize(Uint32 SDLInitFlags);

    SDL_Window *getWindow() const;
    SDL_Renderer *getRenderer() const;

  private:
    SDL_Window *mWindow{nullptr};
    SDL_Renderer *mRenderer{nullptr};
};

#endif // SURVIVAL_WINDOW_H
