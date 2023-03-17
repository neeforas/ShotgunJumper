#include "Window.h" 


Window::Window(const char* p_title, int p_w, int p_h)
        :m_window(NULL), m_renderer(NULL)
{
        m_window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_w, p_h, SDL_WINDOW_SHOWN);
        if (m_window == NULL)
        {
                printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }

        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED && SDL_RENDERER_PRESENTVSYNC);
        if (m_renderer == NULL)
        {
                printf("Not able to create renderer! SDL Error: %s\n", SDL_GetError());
        }
}



bool Window::init()
{
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
                printf("SDL could not be initialized! SDL_Error: %s\n", SDL_GetError());
                return false;
        }

        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
                printf("SDL_image could not be initialized! SDL_image Error: %s\n", IMG_GetError());
                return false;
        }

        return true;
}


void Window::clear()
{
        SDL_RenderClear(m_renderer);
}


void Window::cleanup()
{
        SDL_DestroyWindow(m_window);
        m_window = NULL;

        SDL_DestroyRenderer(m_renderer);
        m_renderer = NULL;
}


void Window::setColor(int p_r, int p_g, int p_b, int p_a)
{
        SDL_SetRenderDrawColor(m_renderer, p_r, p_g, p_b, p_a);
}


void Window::display()
{
        SDL_RenderPresent(m_renderer);
}


SDL_Texture* Window::loadTexture(const char* p_path)      
{  
        SDL_Texture* texture = NULL;


        texture = IMG_LoadTexture(m_renderer, p_path);

        if (texture == NULL)
        {
                std::cout << "Failed to load texture. Error " << SDL_GetError() << std::endl;
        }

        return texture;
}

void Window::RenderCopy(SDL_Texture* p_texture, SDL_Rect p_dst)
{
        SDL_RenderCopy(m_renderer, p_texture, NULL, &p_dst);
}

void Window::RenderCopy(SDL_Texture* p_texture, SDL_Rect p_src, SDL_Rect p_dst)
{
        SDL_RenderCopy(m_renderer, p_texture, &p_src, &p_dst);
}

void Window::RenderCopyEx(SDL_Texture* p_texture, SDL_Rect p_src, SDL_Rect p_dst, float p_angle)
{
        SDL_RenderCopyEx(m_renderer, p_texture, &p_src, &p_dst, p_angle, NULL, SDL_FLIP_NONE);
}
