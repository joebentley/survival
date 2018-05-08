#include <algorithm>
#include "texture.h"

Texture::~Texture() {
    SDL_DestroyTexture(texture);
}

void Texture::render(SDL_Renderer *renderer, SDL_Rect *src, SDL_Rect *dst) {
    SDL_RenderCopy(renderer, texture, src, dst);
}

int Texture::LoadFromFile(Texture &texture, SDL_Renderer* renderer, const std::string& filepath) {
    SDL_Surface* loadedSurface = SDL_LoadBMP(filepath.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", filepath.c_str(), SDL_GetError());
        return -1;
    }
    else {
        texture.texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (texture.texture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n", filepath.c_str(), SDL_GetError());
            return -1;
        }

        SDL_FreeSurface(loadedSurface);
    }
    return 0;
}