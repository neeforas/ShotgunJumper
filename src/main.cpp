/*
Major Goals:
    - Sound

Medium Goals:
    - fix ground glitch

*/





#include <stdio.h>
#include <vector>
#include <math.h>
#include <stdlib.h>     
#include <time.h> 

#include "Window.h"
#include "Entity.h"
#include "Bullet.h"


// settings
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int FPS = 40;
const float SECONDS_PER_FRAME = 1000 / 50;


bool init();
void input();
void update();
void render();
void cleanup();
void gameLoop();

Window g_window("Shotgun Jumper", SCREEN_WIDTH, SCREEN_HEIGHT);
SDL_Event g_event;
bool g_b_gameIsRunning;

// ++==============++
// || GAME CONTENT ||
// ++==============++

int blockSize = 40;
std::vector < Entity > grassBlocks;

bool shooting = false;
bool roundStarted = false;

int speed = 2;
int gravity = 1;
int vsp = 0;
int jumpCounter = 0;
bool lookingDown = false;
int da[2]={0,0};

int hearts = 3;
int ammo = 5;

int startTime = 0;

int timeSinceLastShot;
std::vector < int > shotTimer;

// GRASS
SDL_Texture* grass = g_window.loadTexture("res/gfx/grass_block.png");
SDL_Rect grassRectSrc = {0,0,32,32};

// CLOUDS
SDL_Texture* cloud_tex = g_window.loadTexture("res/gfx/cloud.png");
SDL_Rect cloudRectSrc = {0,0,994,634};

std::vector < SDL_Rect > cloudRects 
{
        {50, 30, 160, 100},
        {500, 90, 60, 40}
};
std::vector < Entity > clouds
{
        Entity(cloud_tex, cloudRectSrc, cloudRects[0]),
        Entity(cloud_tex, cloudRectSrc, cloudRects[1])
};

// CITIES
SDL_Texture* city1_tex = g_window.loadTexture("res/gfx/city1.png");
SDL_Texture* city2_tex = g_window.loadTexture("res/gfx/city2.png");

SDL_Rect city1Src = {0,0,1200,1200};
SDL_Rect city2Src = {0,0,1920,1080};

std::vector < SDL_Rect > cityRects
{
        {0, 260, 180, 180},
        {0, 370, 240, 100},
        {180, 260, 180, 180},
        {240, 370, 150, 100},
        {360, 260, 180, 180},
        {390, 370, 150, 100},
        {540, 260, 180, 180},
        {540, 370, 150, 100},
        {720, 260, 180, 180},
        {720, 370, 150, 100}
};

std::vector < Entity > cities
{
        Entity(city1_tex, city1Src, cityRects[0]),
        Entity(city2_tex, city2Src, cityRects[1]),
        Entity(city1_tex, city1Src, cityRects[2]),
        Entity(city2_tex, city2Src, cityRects[3]),
        Entity(city1_tex, city1Src, cityRects[4]),
        Entity(city2_tex, city2Src, cityRects[5]),
        Entity(city1_tex, city1Src, cityRects[6]),
        Entity(city2_tex, city2Src, cityRects[7]),
        Entity(city1_tex, city1Src, cityRects[8]),
        Entity(city2_tex, city2Src, cityRects[9])
};

SDL_Texture* heart_tex = g_window.loadTexture("res/gfx/heart.png");
SDL_Rect heartSrc = {0,0,1500,1400};
SDL_Rect heartDst = {0,0,0,0}; // changed while rendering 
Entity heart = Entity(heart_tex, heartSrc, heartDst);

SDL_Texture* shell_tex = g_window.loadTexture("res/gfx/shell.png");
SDL_Rect shellSrc = {0,0,356,700};
SDL_Rect shellDst = {0, 0, 0, 0}; // changed while rendering
Entity shell = Entity(shell_tex, shellSrc, shellDst);

float shotgunAngle = 0;
SDL_Texture* shotgun_up_1 = g_window.loadTexture("res/gfx/shotgun_up_1.png");
SDL_Texture* shotgun_up_2 = g_window.loadTexture("res/gfx/shotgun_up_2.png");

SDL_Texture* shotgun_tex = g_window.loadTexture("res/gfx/shotgun.png");
SDL_Rect shotgunSrc = {0,0,700,180};
SDL_Rect shotgunDst = {250,350,140,36};
Entity shotgun = Entity(shotgun_tex, shotgunSrc, shotgunDst);

SDL_Rect verticalShotgun = {300,300,36,140};

SDL_Texture* billy_tex = g_window.loadTexture("res/gfx/billy.png");
SDL_Rect billySrc = {0,0,270,340};
SDL_Rect billyDst = {266,372,54,68};
Entity billy = Entity(billy_tex, billySrc, billyDst);

SDL_Texture* bird_tex1 = g_window.loadTexture("res/gfx/bird/1.png");
SDL_Texture* bird_tex2 = g_window.loadTexture("res/gfx/bird/2.png");
SDL_Texture* bird_tex3 = g_window.loadTexture("res/gfx/bird/3.png");
SDL_Texture* bird_tex4 = g_window.loadTexture("res/gfx/bird/4.png");
std::vector < SDL_Texture* > birdAnimation = {bird_tex1, bird_tex2, bird_tex3, bird_tex4, bird_tex3, bird_tex2};

SDL_Rect birdSrc = {0,0,448,381};
SDL_Rect birdDst = {800,150,40,40};

Entity bird = Entity(bird_tex1, birdSrc, birdDst);

SDL_Texture* flame = g_window.loadTexture("res/gfx/flame.png");



// ++===========================++
// || GAME FUNCTION DEFINITIONS ||
// ++===========================++

float positiveAngle;
std::vector < Bullet > bullets;
void shoot()
{
        SDL_Rect bullet = {290, shotgun.dst.y, 60, 60};
        bullets.push_back(Bullet(NULL, heartSrc, bullet, 0, 0));

        bullets[bullets.size()-1].dx = 20*cos(0.0174533*positiveAngle);
        bullets[bullets.size()-1].dy = 20*sin(0.0174533*positiveAngle);
        bullets[bullets.size()-1].swapTexture(flame);

        if (shotTimer.size()>0)
        {
                timeSinceLastShot = SDL_GetTicks() - shotTimer[shotTimer.size()-1];
        }
        if (timeSinceLastShot > 200 || shotTimer.size() == 0)
        {
                jumpCounter++;
                if (jumpCounter <= 2 && lookingDown && ammo > 0)
                {
                        vsp = -15;
                }
                if (roundStarted && ammo > 0)
                        ammo--;
                shotTimer.push_back(SDL_GetTicks());
        }
}

std::vector < Entity > drops;
bool floorDropGenerated = false;
bool skyDropGenerated = false;
void spawnDrop()
{
        if (ammo == 0 && !floorDropGenerated)
        {
                SDL_Rect rect = {rand()%1000+640, 360, 20, 40};
                drops.push_back(Entity(shell_tex, shellSrc, rect));
                floorDropGenerated = true;
        }
        if (!skyDropGenerated)
        {
                SDL_Rect rect = {rand()%1500+640, 150, 20, 40};
                drops.push_back(Entity(shell_tex, shellSrc, rect));
                skyDropGenerated = true;
        }
}

bool init()
{ 
        srand(time(0));
        if (!g_window.init())
        {
                printf("SDL could not initialize");
                return false;
        }
        g_b_gameIsRunning = true;
        for (int i = 0; i < 17; i++)
        {
                SDL_Rect rect_dst = {i*blockSize, SCREEN_HEIGHT-blockSize, blockSize, blockSize};
                grassBlocks.push_back(Entity(grass, grassRectSrc, rect_dst));
        }

        return true;
        
}

void input()
{
        while(SDL_PollEvent(&g_event) != 0)
        {
                if (g_event.type == SDL_QUIT || g_event.key.keysym.sym == SDLK_ESCAPE)
                        g_b_gameIsRunning = false;

                if (g_event.type == SDL_KEYDOWN)
                {
                        switch (g_event.key.keysym.sym)
                        {
                                case SDLK_w:
                                        if (!shooting && roundStarted)
                                        {
                                                if (ammo > 0)
                                                {
                                                        shooting = true;
                                                        shoot();  
                                                }
                                        }
                                        break;
                                case SDLK_a:
                                        da[1] = 1;
                                        break;
                                case SDLK_d:
                                        da[0] = 1;
                                        break;
                                case SDLK_UP:
                                        if (!shooting && roundStarted)
                                        {
                                                if (ammo > 0)
                                                {
                                                        shooting = true;
                                                        shoot();  
                                                }
                                                   
                                        }
                                        break;
                                case SDLK_LEFT:
                                        da[1] = 1;
                                        break;
                                case SDLK_RIGHT:
                                        da[0] = 1;
                                        break;
                                default:
                                        break;
                        }
                        if (!roundStarted)
                                startTime = SDL_GetTicks();
                            {roundStarted = true;}
                }

                if (g_event.type == SDL_KEYUP)
                {
                        switch (g_event.key.keysym.sym)
                        {
                                case SDLK_w:
                                        shooting = false;
                                        break;
                                case SDLK_a:
                                        da[1] = 0;
                                        break;
                                case SDLK_d:
                                        da[0] = 0;
                                        break;
                                case SDLK_UP:
                                        shooting = false;
                                        break;
                                case SDLK_LEFT:
                                        da[1] = 0;
                                        break;
                                case SDLK_RIGHT:
                                        da[0] = 0;
                                        break;
                                default:
                                        break;
                        }
                }
        }
}

int animationTime = 600;
void update()
{
        if (hearts == 0)
                g_b_gameIsRunning = false;
        if (shotgunAngle < 0)
        {
                positiveAngle = 360 + (int(shotgunAngle) % 360);
        }
        else
        {
                positiveAngle = int(shotgunAngle) % 360;
        }

        for (int i = 0; i < bullets.size(); i++)
        {
                if (bullets[i].dst.x+bullets[i].dst.w<0 || bullets[i].dst.x>SCREEN_WIDTH ||
                    bullets[i].dst.y+bullets[i].dst.h<0 || bullets[i].dst.y>SCREEN_HEIGHT)
                    {
                        bullets.erase(bullets.begin() + i);
                    }
                if (bullets[i].dx==0 && bullets[i].dy==0)
                {
                        bullets[i].dst.y = shotgun.dst.y;
                }
                bullets[i].dst.x += bullets[i].dx;
                bullets[i].dst.y -= bullets[i].dy;
                if (SDL_HasIntersection(&bullets[i].dst, &bird.dst))
                {
                        bird.dst.x = 1200;
                        bird.dst.y = rand()%100 + 150;
                        bullets.erase(bullets.begin() + i);
                }
        }

        if (SDL_GetTicks()%animationTime < animationTime/6)
                bird.swapTexture(bird_tex1);
        else if (SDL_GetTicks()%animationTime < animationTime/6*2)
                bird.swapTexture(bird_tex2);
        else if (SDL_GetTicks()%animationTime < animationTime/6*3)
                bird.swapTexture(bird_tex3);
        else if (SDL_GetTicks()%animationTime < animationTime/6*4)
                bird.swapTexture(bird_tex4);
        else if (SDL_GetTicks()%animationTime < animationTime/6*5)
                bird.swapTexture(bird_tex3);
        else if (SDL_GetTicks()%animationTime < animationTime)
                bird.swapTexture(bird_tex2);
        
        if (roundStarted)
        {
                bird.dst.x -= 3*speed;

                for (int i = 0; i<grassBlocks.size(); i++)
                {
                        grassBlocks[i].dst.x -= speed;
                        if (grassBlocks[i].dst.x + grassBlocks[i].dst.w < 1)
                        {
                                grassBlocks[i].dst.x = SCREEN_WIDTH;
                        }
                }

                for (int i=0; i<clouds.size(); i++)
                {
                        clouds[i].dst.x -= 1;
                        clouds[i].dst.x -= 1;

                        if (clouds[i].dst.x < - clouds[i].dst.w)
                        {
                                clouds[i].dst.x = SCREEN_WIDTH + clouds[i-1].dst.w;
                        }  
                }
                
                for (int i=0; i<cities.size(); i++)
                {   
                        cities[i].dst.x -= 1;    
                }  

                for (int i=0; i<drops.size(); i++)
                {   
                        if (drops[i].dst.x + drops[i].dst.w < 0)
                        {
                                drops.erase(drops.begin()+i);
                                skyDropGenerated = false;
                        }
                        else
                        {
                                drops[i].dst.x -= 2*speed;
                        }
                        if (SDL_HasIntersection(&drops[i].dst, &shotgun.dst))
                        {
                                drops.erase(drops.begin()+i);
                                ammo += 5;
                                if (shotgun.dst.y == 350)
                                        floorDropGenerated = false;
                                else
                                        skyDropGenerated = false;
                        }   
                } 
        }

        if (da[1]==1)
        {
                shotgunAngle += 10;
        }
        if (da[0]==1)
        {
                shotgunAngle -= 10;
        }

        billy.dst.y += vsp;
        shotgun.dst.y += vsp;
        verticalShotgun.y += vsp;

        bool inTheAir = billy.dst.y + billy.dst.h < SCREEN_HEIGHT - blockSize;
        if (inTheAir)
        {
                vsp += gravity;
        }

        bool touchingGround = billy.dst.y + billy.dst.h >= SCREEN_HEIGHT - blockSize;
        if (touchingGround)
        {
                billy.dst.y = SCREEN_HEIGHT - blockSize - billy.dst.h;
                shotgun.dst.y = 350;
                verticalShotgun.y = 300;
                vsp = 0;
                jumpCounter = 0;
        }         

        lookingDown = (int(shotgunAngle) % 360 > 225 && int(shotgunAngle) % 360 < 315) ||
        (shotgunAngle<0 && int(-shotgunAngle) % 360 > 45 && int(-shotgunAngle) % 360 < 125);

        clouds[0].dst.y = 30 + 2*sin( (SDL_GetTicks() / 100 )  ) ;
        clouds[1].dst.y = 90 + sin( (SDL_GetTicks() / 100 + 200 ) );

        for (int i = 0; i < cities.size(); i++)
        {
                if (cities[i].dst.x+cities[i].dst.w<0)
                {
                        cities[i].dst.x = SCREEN_WIDTH;
                }
        }

        if (bird.dst.x + bird.dst.w < 0)
        {
                bird.dst.x = 1200;
                bird.dst.y = rand()%100 + 150;
                hearts--;
        }
        if (ammo > 15)
                ammo = 15;
}


void render()
{
        // clears the screen
        g_window.clear();

        // Sets background color
        g_window.setColor(146,185,235,255);

        // Add rendering content below

        for (int i = 0; i < hearts; i++)
        {
                SDL_Rect heartRect = {i*30+10, 10, 30, 28};
                g_window.RenderCopy(heart.texture, heart.src, heartRect);
        }

        for (int i = 0; i < ammo; i++)
        {
                SDL_Rect shellRect = {SCREEN_WIDTH-(16 * (i+1)) - 10, 10, 14, 28};
                g_window.RenderCopy(shell.texture, shell.src, shellRect);
        }

        for (Entity city : cities)
        {       
                if (city.dst.x + city.dst.w > 0 && city.dst.x < SCREEN_WIDTH)
                {
                        g_window.RenderCopy(city.texture, city.src, city.dst);
                }
        }

        for (Entity cloud : clouds)
        {
                if (cloud.dst.x + cloud.dst.w > 0 && cloud.dst.x < SCREEN_WIDTH)
                {
                        g_window.RenderCopy(cloud.texture, cloud.src, cloud.dst);
                }
        }

        for (int i = 0; i < grassBlocks.size(); i++)
        {
                if (grassBlocks[i].dst.x + grassBlocks[i].dst.w > 0 && grassBlocks[i].dst.x < SCREEN_WIDTH)
                {
                        g_window.RenderCopy(grassBlocks[i].texture, grassBlocks[i].src, grassBlocks[i].dst);
                } 
        }

        for (int i=0; i<drops.size(); i++)
        {
                if (drops[i].dst.x + drops[i].dst.w > 0 && drops[i].dst.x < SCREEN_WIDTH)
                {
                        g_window.RenderCopy(drops[i].texture, drops[i].src, drops[i].dst);
                } 
        }

        g_window.RenderCopy(bird.texture, bird.src, bird.dst);

        g_window.setColor(0,0,0,255);
        //SDL_RenderDrawRect(g_window.m_renderer, &shotgun.dst);
        //SDL_RenderDrawRect(g_window.m_renderer, &bird.dst);
        //SDL_RenderDrawRect(g_window.m_renderer, &clouds[0].dst);
        //SDL_RenderDrawRect(g_window.m_renderer, &clouds[1].dst);
        //SDL_RenderDrawRect(g_window.m_renderer, &verticalShotgun);
        for (int i = 0; i < bullets.size(); i++)
        {
                //SDL_RenderDrawRect(g_window.m_renderer, &bullets[i].dst);
                g_window.RenderCopy(bullets[i].texture, bullets[i].src, bullets[i].dst);
        }
        g_window.RenderCopyEx(shotgun.texture, shotgun.src, shotgun.dst, -shotgunAngle);
        g_window.setColor(146,185,235,255);
        
        g_window.display();
}


void gameLoop()
{
        int frameStart = SDL_GetTicks();

        spawnDrop();
        input();
        
        update();
        
        render();

        int frameEnd = SDL_GetTicks();
        int frameTime = frameEnd - frameStart;

        if (frameTime < SECONDS_PER_FRAME)
        {
                SDL_Delay(SECONDS_PER_FRAME - frameTime);
        }

}


void cleanup()
{
        SDL_Quit();
        g_window.cleanup();
}


int main(int argc, char* argv[]) 
{
        if (!init())
        {
                printf("Could not initialize!\n");
                return -1;
        }

        while (g_b_gameIsRunning)
        {
           gameLoop();
        }

        cleanup();

        return 0;
}

