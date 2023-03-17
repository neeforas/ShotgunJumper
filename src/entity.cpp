#include "Entity.h"

Entity::Entity(SDL_Texture* p_tex, SDL_Rect p_src, SDL_Rect p_dst)
{
    texture = p_tex;
    src = p_src;
    dst = p_dst;
}

void Entity::swapTexture(SDL_Texture* p_tex)
{
    texture = p_tex;
}