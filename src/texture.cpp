#include <algorithm>
#include <SDL2/SDL_image.h>
#include "texture.h"
#include "world.h"

Texture::~Texture() {
    SDL_DestroyTexture(mTexture);
}

void Texture::render(SDL_Renderer *renderer, SDL_Rect *src, SDL_Rect *dst) {
    SDL_RenderCopy(renderer, mTexture, src, dst);
}

int Texture::LoadFromFile(Texture &texture, SDL_Renderer* renderer, const std::string& filepath) {
    SDL_Surface* loadedSurface = IMG_Load(filepath.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", filepath.c_str(), SDL_GetError());
        return -1;
    }
    else {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0, 0xFF));

        texture.mTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (texture.mTexture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n", filepath.c_str(), SDL_GetError());
            return -1;
        }

        SDL_FreeSurface(loadedSurface);
    }

    SDL_QueryTexture(texture.mTexture, &texture.mFormat, &texture.mAccess, &texture.mWidth, &texture.mHeight);

    return 0;
}

LightMapTexture::~LightMapTexture() {
    SDL_DestroyTexture(mNightFadeTexture);
}

int LightMapTexture::load() {
    return Texture::LoadFromFile(mLightTexture, mRenderer, "resources/light.png");
}

void LightMapTexture::init() {
    mNightFadeTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, CHAR_WIDTH * SCREEN_WIDTH, CHAR_HEIGHT * SCREEN_HEIGHT);
    SDL_SetTextureBlendMode(mNightFadeTexture, SDL_BLENDMODE_MOD);
    SDL_SetTextureBlendMode(mLightTexture.mTexture, SDL_BLENDMODE_ADD);
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(mRenderer);
}

void LightMapTexture::render(std::vector<LightMapPoint> points, Uint8 backgroundAlpha) {
    SDL_SetRenderTarget(mRenderer, mNightFadeTexture);
    SDL_SetRenderDrawColor(mRenderer, backgroundAlpha, backgroundAlpha, backgroundAlpha, 0xFF);
    SDL_RenderFillRect(mRenderer, nullptr);

    for (const auto &point : points) {
        SDL_Rect rect { point.mPoint.mX - point.mRadius, point.mPoint.mY - point.mRadius, 2 * point.mRadius, 2 * point.mRadius };
        SDL_SetTextureColorMod(mLightTexture.mTexture, point.mColor.r, point.mColor.g, point.mColor.b);
        mLightTexture.render(mRenderer, nullptr, &rect);
    }

    SDL_SetRenderTarget(mRenderer, nullptr);
    SDL_RenderCopy(mRenderer, mNightFadeTexture, nullptr, nullptr);
}
