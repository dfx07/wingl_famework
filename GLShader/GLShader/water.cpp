//Implemented by http://www.youtube.com/user/thecplusplusguy
//The algorithm: http://freespace.virgin.net/hugo.elias/graphics/x_water.htm
//SDL library used
//You need a 640x480 image (more colorful the better), called waterback.bmp in the same folder
#include <iostream>
#include <SDL/SDL.h>
using namespace std;

const int WIDTH = 640;
const int HEIGHT = 480;
const int FPS = 60;
int dumpv2 = 8;
int WATERHIGHT = 200;
const float dump = 0.99;
int** buffer1;
int** buffer2;


void putPixel(SDL_Surface* surf, int x, int y, int r, int g, int b)
{
    if (x >= 0 && y >= 0 && x<WIDTH && y<HEIGHT)
    {
        (((Uint32*)(surf->pixels))[y*surf->pitch / 4 + x]) = SDL_MapRGB(surf->format, r, g, b);
    }
}

void putPixel(SDL_Surface* surf, int x, int y, Uint32 col)
{
    if (x >= 0 && y >= 0 && x<WIDTH && y<HEIGHT)
    {
        ((Uint32*)(surf->pixels))[y*surf->pitch / 4 + x] = col;
    }
}

Uint32 getPixel(SDL_Surface* surf, int x, int y)
{
    if (x<0)
        x = 0;
    if (y<0)
        y = 0;
    if (x >= WIDTH)
        x = WIDTH - 1;
    if (y >= HEIGHT)
        y = HEIGHT - 1;
    return (((Uint32*)(surf->pixels))[y*surf->pitch / 4 + x]);
}

void refreshWater()
{
    for (int i = 1; i<HEIGHT - 1; i++)
        for (int j = 1; j<WIDTH - 1; j++)
        {
            buffer2[i][j] = ((buffer1[i][j - 1] + buffer1[i][j + 1] + buffer1[i - 1][j] + buffer1[i + 1][j]) >> 1) - buffer2[i][j];
            //      buffer2[i][j]*=dump;
            buffer2[i][j] -= buffer2[i][j] >> dumpv2;
        }
    int **tmp;
    tmp = buffer2;
    buffer2 = buffer1;
    buffer1 = tmp;
}

void renderWater(SDL_Surface* screen, SDL_Surface* backImg)
{
    for (int i = 1; i<HEIGHT - 1; i++)
        for (int j = 1; j<WIDTH - 1; j++)
        {
            int diffY = buffer1[i - 1][j] - buffer1[i + 1][j];
            int diffX = buffer1[i][j - 1] - buffer1[i][j + 1];
            //      cout << diffX << " " << diffY << endl;
            putPixel(screen, j, i, getPixel(screen, j + diffY, i + diffX));
        }
}


int main(int argc, char** argv)
{
    buffer1 = (int**)new int[HEIGHT];
    buffer2 = (int**)new int[HEIGHT];
    for (int i = 0; i<HEIGHT; i++)
    {
        buffer1[i] = new int[WIDTH];
        buffer2[i] = new int[WIDTH];
    }
    for (int i = 0; i<HEIGHT; i++)
        for (int j = 0; j<WIDTH; j++)
            buffer1[i][j] = buffer2[i][j] = 0;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Surface* screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);
    SDL_Surface* backImgTmp = SDL_LoadBMP("waterback.bmp");
    SDL_Surface* backImg = SDL_DisplayFormat(backImgTmp);
    if (!backImg)
        cout << "error with the image" << endl;
    SDL_FreeSurface(backImgTmp);
    SDL_Event event;
    bool running = true;
    Uint32 start;
    int x, y;
    int num = 10;
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
                case SDLK_q:
                    num++;
                    break;
                case SDLK_a:
                    if (num>0)
                        num--;
                    break;
                case SDLK_w:
                    WATERHIGHT += 20;
                    break;
                case SDLK_s:
                    if (WATERHIGHT>20)
                        WATERHIGHT -= 20;
                    break;
                }
                break;
            }
        }
        //if(rand()%10<8)
        for (int i = 0; i<num; i++)
            buffer1[rand() % HEIGHT][rand() % WIDTH] += rand() % WATERHIGHT + 1;
        SDL_BlitSurface(backImg, NULL, screen, NULL);
        refreshWater();
        renderWater(screen, backImg);

        SDL_Flip(screen);
        if (1000.0 / FPS>(SDL_GetTicks() - start))
            SDL_Delay(1000.0 / FPS - (SDL_GetTicks() - start));
    }
    for (int i = 0; i<HEIGHT; i++)
    {
        delete[] buffer1[i];
        delete[] buffer2[i];
    }
    delete[] buffer1;
    delete[] buffer2;
    SDL_FreeSurface(backImg);
    SDL_Quit();
}