#include <algorithm>

#ifdef XCODE
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2_/SDL_image.h>
#endif

#include "Texture.h"
#include "SDLManager.h"
#include "World.h"

Texture::~Texture() {
    if (mTexture != nullptr)
        SDL_DestroyTexture(mTexture);
}

void Texture::render(SDL_Rect *src, SDL_Rect *dst) {
    SDL_RenderCopy(mRenderer, mTexture, src, dst);
}

int Texture::loadFromFile(const std::string &filepath) {
    SDL_Surface* loadedSurface = IMG_Load(filepath.c_str());
    if (loadedSurface == nullptr) {
        printf("Unable to load image %s! SDL_image Error: %s\n", filepath.c_str(), SDL_GetError());
        return -1;
    }
    else {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0, 0xFF));

        mTexture = SDL_CreateTextureFromSurface(mRenderer, loadedSurface);
        if (mTexture == nullptr) {
            printf("Unable to create texture from %s! SDL Error: %s\n", filepath.c_str(), SDL_GetError());
            return -1;
        }

        SDL_FreeSurface(loadedSurface);
    }

    SDL_QueryTexture(mTexture, &mFormat, &mAccess, &mWidth, &mHeight);

    return 0;
}

SDL_Texture *Texture::getTexture() const {
    return mTexture;
}

Uint32 Texture::getFormat() const {
    return mFormat;
}

int Texture::getAccess() const {
    return mAccess;
}

int Texture::getWidth() const {
    return mWidth;
}

int Texture::getHeight() const {
    return mHeight;
}

LightMapTexture::~LightMapTexture() {
    if (mNightFadeTexture != nullptr)
        SDL_DestroyTexture(mNightFadeTexture);
}

LightMapTexture::LightMapTexture(SDL_Renderer *renderer) : Texture(renderer) {
    loadFromFile("resources/light.png");
    mNightFadeTexture = SDL_CreateTexture(mRenderer, mFormat, SDL_TEXTUREACCESS_TARGET,
            WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetTextureBlendMode(mNightFadeTexture, SDL_BLENDMODE_MOD);
    SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_ADD);
}

void LightMapTexture::render(std::vector<LightMapPoint> points, Uint8 backgroundAlpha) {
    SDL_SetRenderTarget(mRenderer, mNightFadeTexture);
    SDL_SetRenderDrawColor(mRenderer, backgroundAlpha, backgroundAlpha, backgroundAlpha, 0xFF);
    SDL_RenderFillRect(mRenderer, nullptr);

    for (const auto &point : points) {
        SDL_Rect rect { point.mPoint.mX - point.mRadius, point.mPoint.mY - point.mRadius, 2 * point.mRadius, 2 * point.mRadius };
        SDL_SetTextureColorMod(mTexture, point.mColor.r, point.mColor.g, point.mColor.b);
        Texture::render(nullptr, &rect);
    }

    SDL_SetRenderTarget(mRenderer, nullptr);
    SDL_RenderCopy(mRenderer, mNightFadeTexture, nullptr, nullptr);
}
