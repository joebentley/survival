#include <SDL2/SDL.h>
#include <string>

#ifndef TEXTURE_H_
#define TEXTURE_H_

class Texture {
public:
    SDL_Texture *texture;

    Texture() : texture (NULL) {};
    ~Texture();

    void render(SDL_Renderer *renderer, SDL_Rect *src, SDL_Rect *dst);

    static int LoadFromFile(Texture &texture, SDL_Renderer* renderer, const std::string& filepath);
};

#endif // TEXTURE_H_