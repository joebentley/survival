#include <algorithm>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "SDLManager.h"
#include "Texture.h"
#include "World.h"

Texture::~Texture() {
    if (mTexture != nullptr)
        SDL_DestroyTexture(mTexture);
}

void Texture::render(SDL_FRect *src, SDL_FRect *dst) { SDL_RenderTexture(mRenderer, mTexture, src, dst); }

int Texture::loadFromFile(const std::string &filepath) {
    SDL_Surface *loadedSurface = IMG_Load(filepath.c_str());
    if (loadedSurface == nullptr) {
        printf("Unable to load image %s! SDL_image Error: %s\n", filepath.c_str(), SDL_GetError());
        return -1;
    } else {
        SDL_SetSurfaceColorKey(loadedSurface, true, SDL_MapSurfaceRGB(loadedSurface, 0xFF, 0, 0xFF));

        mTexture = SDL_CreateTextureFromSurface(mRenderer, loadedSurface);
        if (mTexture == nullptr) {
            printf("Unable to create texture from %s! SDL Error: %s\n", filepath.c_str(), SDL_GetError());
            return -1;
        }

        SDL_DestroySurface(loadedSurface);
    }

    return 0;
}

SDL_Texture *Texture::getTexture() const { return mTexture; }

SDL_PixelFormat Texture::getFormat() const { return mTexture->format; }

int Texture::getWidth() const { return mTexture->w; }

int Texture::getHeight() const { return mTexture->h; }
