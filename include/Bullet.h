#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

class Bullet : public Entity
{
public:
    int dx, dy;
    Bullet(SDL_Texture* p_tex, SDL_Rect p_src, SDL_Rect p_dst, int p_dx, int p_dy)
    : Entity(p_tex, p_src, p_dst), dx(p_dx), dy(p_dy)
    {}
};