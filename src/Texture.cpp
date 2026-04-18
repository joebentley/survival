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

LightMapTexture::~LightMapTexture() {
    if (mNightFadeTexture != nullptr)
        SDL_DestroyTexture(mNightFadeTexture);
}

LightMapTexture::LightMapTexture(SDL_Renderer *renderer) : Texture(renderer) {
    loadFromFile(std::string(SDL_GetBasePath()) + "/resources/light.png");
    mNightFadeTexture =
        SDL_CreateTexture(mRenderer, getFormat(), SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetTextureBlendMode(mNightFadeTexture, SDL_BLENDMODE_MOD);
    SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_ADD);
}

void LightMapTexture::render(std::vector<LightMapPoint> points, Uint8 backgroundAlpha) {
    SDL_SetRenderTarget(mRenderer, mNightFadeTexture);
    SDL_SetRenderDrawColor(mRenderer, backgroundAlpha, backgroundAlpha, backgroundAlpha, 0xFF);
    SDL_RenderFillRect(mRenderer, nullptr);

    for (const auto &point : points) {
        SDL_FRect rect{
            static_cast<float>(point.mPoint.mX - point.mRadius),
            static_cast<float>(point.mPoint.mY - point.mRadius),
            static_cast<float>(2 * point.mRadius),
            static_cast<float>(2 * point.mRadius),
        };
        SDL_SetTextureColorMod(mTexture, point.mColor.r, point.mColor.g, point.mColor.b);
        Texture::render(nullptr, &rect);
    }

    SDL_SetRenderTarget(mRenderer, nullptr);
    SDL_RenderTexture(mRenderer, mNightFadeTexture, nullptr, nullptr);
}
