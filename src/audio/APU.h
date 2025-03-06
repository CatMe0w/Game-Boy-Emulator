#pragma once

#include <cstdint>
#include <cstring>
#include <math.h>

#include "bus.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_timer.h>

// unfinished
namespace GBC {
    struct square {
        int sample_nr = 0;
        uint8_t duty = 0;
        float tone = 0;
        float amplitude = 0;
    };
    
    class APU {
        byte div, div_prev;
        uint32_t SAMPLE_RATE = 44100;

        void execute_cycle();

        float square_wave(double n, uint8_t duty);

        void square_callback(void *user_data, Uint8 *raw_buffer, int bytes);
        void SDLCALL sdl_audio_callback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount);
    };
}

