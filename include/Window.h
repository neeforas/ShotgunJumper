#pragma once

#include <iostream>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

class Window
{
public:
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	Window(const char* p_title, int p_w, int p_h);
	bool init();
	void clear();
	void cleanup();

	void setColor(int p_r, int p_g, int p_b, int p_a);
	void display();

	SDL_Texture* loadTexture(const char* p_path);       

	void RenderCopy(SDL_Texture* p_texture, SDL_Rect p_dst);
	void RenderCopy(SDL_Texture* p_texture, SDL_Rect p_src, SDL_Rect p_dst);
	void RenderCopyEx(SDL_Texture* p_texture, SDL_Rect p_src, SDL_Rect p_dst, float p_angle);


};