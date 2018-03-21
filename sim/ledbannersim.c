#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "SDL.h"
#include "SDL_video.h"

#define WIDTH 36
#define HEIGHT 36
#define DEPTH 24
#define PIXELS (WIDTH * HEIGHT)
#define BYTES (PIXELS * 3)

#define BIGGER  10

static uint8_t frame[HEIGHT][WIDTH][3];

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    SDL_Init(SDL_INIT_VIDEO);
    //SDL_Renderer *renderer;

    // get window surface
    SDL_Window *window = 
        SDL_CreateWindow("ledbanner", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH*BIGGER, HEIGHT*BIGGER, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Surface *surface = SDL_GetWindowSurface(window);

    // create surface for led banner frame
    SDL_Surface *frame_surface = 
        SDL_CreateRGBSurfaceFrom(frame, WIDTH, HEIGHT, DEPTH, WIDTH*3, 0x0000FF, 0x00FF00, 0xFF0000, 0);

    SDL_RendererFlip flip = SDL_FLIP_HORIZONTAL;



    // main loop
    int c;
    while ((c = read(0, frame, BYTES))) {

        SDL_BlitScaled(frame_surface, NULL, surface, NULL);
        
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopyEx(renderer, texture, NULL, NULL, 90, NULL, flip);
        SDL_RenderPresent(renderer);
        
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                printf("Quit received...");
                break;
            }
        }
    }    
    
    SDL_FreeSurface(frame_surface);
    SDL_Quit();
    printf("Quit!\n");
    return 0;
}

