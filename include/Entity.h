#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

class Entity
{
public:
    SDL_Texture* texture;
    SDL_Rect src;
    SDL_Rect dst;

    Entity(SDL_Texture* p_tex, SDL_Rect p_src, SDL_Rect p_dst);
    void swapTexture(SDL_Texture* p_tex);
};