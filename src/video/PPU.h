#pragma once 

#include <cstdint>
#include <bitset>

#include "bus.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>



namespace GBC {
    constexpr uint32_t WINDOW_WIDTH = 160;
    constexpr uint32_t WINDOW_HEIGHT = 144;

    struct obj {
        obj() {}
        obj(byte objx, byte objy, byte index, byte flags) : objx(objx), objy(objy), index(index), flags(flags) {}
        byte objx, objy, index, flags;
    } ;

    class PPU {
        public:
        PPU(address_bus *bus) : bus(bus) {  }
        ~PPU();

        state mode = hblank;
        address_bus *bus;
        int dots = 0, lines = 0, renderX = 0, wly = 0;
        bool wlyenabled = 0;


        SDL_Event event;
        SDL_Renderer *renderer, 
                     *debug_tile_renderer, 
                     *debug_window_renderer,
                     *debug_object_renderer,
                     *debug_bg_renderer;
        SDL_Window *window, 
                   *debug_tile_window, 
                   *debug_window_window,
                   *debug_object_window,
                   *debug_bg_window;

        byte frame[160*144];
        bool debug_render = 0, 
             debug_callback = 0;


        void execute_cycle();
        void draw_pixel();
        void dump_info();
        void dump_vram();

        void init_window();

        void init_debug_window();
        void render_debug();

        private: 
        obj objbuffer[10];
        uint8_t objnum = 0;
        inline byte objFIFO();
        inline byte bgFIFO(half tilex, half tiley);
        inline byte windowFIFO(half tilex, half tiley);
    };
}