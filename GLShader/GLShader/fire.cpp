#include <iostream>
#include <SDL/SDL.h>
using namespace std;
const int FPS = 30;
const int WIDTH = 640;
const int HEIGHT = 480;
const float dump = 0.5;
Uint32 colors[100];
float fire[HEIGHT][WIDTH];

void generateFire()
{
    for (int i = 0; i<WIDTH; i++)
    {
        int ran = rand() % 50;
        if (ran == 0)
            fire[HEIGHT - 1][i] = 0;
        if (ran == 1)
            fire[HEIGHT - 1][i] = 99;
    }
    for (int i = 0; i<HEIGHT - 1; i++)
        for (int j = 1; j<WIDTH - 1; j++)
        {
            fire[i][j] = ((fire[i][j] + fire[i + 1][j + 1] + fire[i + 1][j] + fire[i + 1][j - 1]) / 4) - dump;
            if (fire[i][j]<0)
                fire[i][j] = 0;
            if (fire[i][j] >= 100)
                fire[i][j] = 99;
        }
}

void renderFire(SDL_Surface* screen)
{
    for (int i = 0; i<HEIGHT; i++)
        for (int j = 0; j<WIDTH; j++)
        {
            ((Uint32*)screen->pixels)[i*screen->pitch / 4 + j] = colors[(int)fire[i][j]];
        }
}

void interpolate(SDL_Surface* screen, int lindex, int hindex, int str, int stg, int stb, int endr, int endg, int endb)
{
    float stepr, stepg, stepb;
    stepr = (endr - str) / (hindex - lindex);
    stepg = (endg - stg) / (hindex - lindex);
    stepb = (endb - stb) / (hindex - lindex);
    float curr = str, curg = stg, curb = stb;
    for (int i = lindex; i<hindex; i++)
    {
        colors[i] = SDL_MapRGB(screen->format, (int)curr, (int)curg, (int)curb);
        curr += stepr;
        curg += stepg;
        curb += stepb;
    }
}



int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Surface* screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);
    bool running = true;
    SDL_Event event;
    Uint32 start;

    colors[0] = SDL_MapRGB(screen->format, 0, 0, 0);
    interpolate(screen, 0, 35, 0, 0, 0, 255, 0, 0);
    interpolate(screen, 34, 65, 255, 0, 0, 255, 160, 0);
    interpolate(screen, 64, 100, 255, 160, 0, 255, 255, 255);
    for (int i = 0; i<HEIGHT; i++)
        for (int j = 0; j<WIDTH; j++)
            fire[i][j] = 0;
    while (running)
    {
        start = SDL_GetTicks();
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                }
                break;
            }
        }
        SDL_FillRect(screen, NULL, 0);
        generateFire();
        renderFire(screen);

        SDL_Flip(screen);
        if (1000.0 / FPS>(SDL_GetTicks() - start))
            SDL_Delay(1000.0 / FPS - (SDL_GetTicks() - start));
    }
    SDL_Quit();
}