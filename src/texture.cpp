#include <algorithm>
#include <SDL2/SDL_image.h>
#include "texture.h"
#include "world.h"

Texture::~Texture() {
    SDL_DestroyTexture(texture);
}

void Texture::render(SDL_Renderer *renderer, SDL_Rect *src, SDL_Rect *dst) {
    SDL_RenderCopy(renderer, texture, src, dst);
}

int Texture::LoadFromFile(Texture &texture, SDL_Renderer* renderer, const std::string& filepath) {
    SDL_Surface* loadedSurface = IMG_Load(filepath.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", filepath.c_str(), SDL_GetError());
        return -1;
    }
    else {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0, 0xFF));

        texture.texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (texture.texture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n", filepath.c_str(), SDL_GetError());
            return -1;
        }

        SDL_FreeSurface(loadedSurface);
    }

    SDL_QueryTexture(texture.texture, &texture.format, &texture.access, &texture.width, &texture.height);

    return 0;
}

LightMapTexture::~LightMapTexture() {
    SDL_DestroyTexture(nightFadeTexture);
}

int LightMapTexture::load() {
    return Texture::LoadFromFile(lightTexture, renderer, "resources/light.png");
}

void LightMapTexture::init() {
    nightFadeTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, CHAR_WIDTH * SCREEN_WIDTH, CHAR_HEIGHT * SCREEN_HEIGHT);
    SDL_SetTextureBlendMode(nightFadeTexture, SDL_BLENDMODE_MOD);
    SDL_SetTextureBlendMode(lightTexture.texture, SDL_BLENDMODE_ADD);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(renderer);
}

void LightMapTexture::render(std::vector<LightMapPoint> points, Uint8 backgroundAlpha) {
    SDL_SetRenderTarget(renderer, nightFadeTexture);
    SDL_SetRenderDrawColor(renderer, backgroundAlpha, backgroundAlpha, backgroundAlpha, 0xFF);
    SDL_RenderFillRect(renderer, nullptr);

    for (const auto &point : points) {
        SDL_Rect rect { point.p.x - point.radius, point.p.y - point.radius, 2 * point.radius, 2 * point.radius };
        SDL_SetTextureColorMod(lightTexture.texture, point.color.r, point.color.g, point.color.b);
        lightTexture.render(renderer, nullptr, &rect);
    }

    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderCopy(renderer, nightFadeTexture, nullptr, nullptr);
}
