#pragma once 

#include <cstdint>
#include <bitset>

#include "bus.h"


#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

const uint32_t WINDOW_WIDTH = 160;
const uint32_t WINDOW_HEIGHT = 144;


namespace GBC {
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
        int dots = 0, lines = 0, renderX = 0;

        obj objbuffer[10];
        uint8_t objnum = 0;

        SDL_Event event;
        SDL_Renderer *renderer;
        SDL_Window *window;

        byte frame[160*144];
        bool debug=0;

        void execute_cycle();
        void draw_pixel();
        void dump_info();
        void dump_vram();

        void init_window();
        void render_debug();
        byte objFIFO();
        byte bgFIFO();
        byte windowFIFO();
    };
}