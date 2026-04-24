#include "LightMapTexture.h"
#include "LightMapPoint.h"
#include "SDLManager.h"

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
    auto oldRenderTarget = SDL_GetRenderTarget(mRenderer);
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

    SDL_SetRenderTarget(mRenderer, oldRenderTarget);
    SDL_RenderTexture(mRenderer, mNightFadeTexture, nullptr, nullptr);
}
