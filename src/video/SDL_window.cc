#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

const uint32_t WINDOW_WIDTH = 160;
const uint32_t WINDOW_HEIGHT = 144;

namespace GBC {
    class window {
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    int i;

    void run() {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_CreateWindowAndRenderer("(GBC) hello window", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        

        
        while (1) {
            for (i = 0; i < WINDOW_WIDTH; ++i) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderPoint(renderer, i, i);
            }

                SDL_RenderPresent(renderer);
            if (SDL_PollEvent(&event) && event.type == SDL_EVENT_QUIT)
                break;
        }

        }

    };
}