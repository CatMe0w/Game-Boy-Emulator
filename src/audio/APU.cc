#include "APU.h"

// unfinished
namespace GBC {
    void APU::execute_cycle() {

    }

    void channel1();
    void channel2();
    void channel3();
    void channel4();

    float APU::square_wave(double n, uint8_t duty) {
        switch(duty) {
            case 0:
                return ((int)n%8 == 0) ? -1.0f : 1.0f;
            case 1:
                return ((int)n%8 < 2) ? -1.0f : 1.0f;
    
            case 2:
                return ((int)n%8 < 4) ? 1.0f : -1.0f;
            case 3:
                return ((int)n%8 < 2) ? 1.0f : -1.0f;
            default: 
                throw std::runtime_error("should not be here");
        }
    }

    void APU::square_callback(void *user_data, Uint8 *raw_buffer, int bytes) {
        float *buffer = (float*)raw_buffer;
        int length = bytes / 4; // 2 bytes per sample for AUDIO_S16SYS
        square &info(*(square*)user_data);
        int &sample_nr = info.sample_nr;
        float tone = info.tone;
        float amplitude = info.amplitude;


        for(int i = 0; i < length; i++, sample_nr++) {
            double time = (double)sample_nr / (double)SAMPLE_RATE;
            buffer[i] = (float)(info.amplitude * square_wave( 8*tone * time, info.duty)); // render 441 HZ sine wave
        }
    }

    void SDLCALL APU::sdl_audio_callback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount) {
        if (additional_amount > 0) {
            Uint8 *data = SDL_stack_alloc(Uint8, additional_amount);
            if (data) {
                square_callback(userdata, data, additional_amount);
                SDL_PutAudioStreamData(stream, data, additional_amount);
                SDL_stack_free(data);
            }
        }
    }
}