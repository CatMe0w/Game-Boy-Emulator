#pragma once 

#include <stdlib.h>
#include <assert.h>
#include <cstring>

#include "CPU/SM83.h"

#include "MMU/bus.h"
#include "video/PPU.h"
#include "bus.h"

#include <thread>

namespace GBC {
    using namespace std::chrono_literals;

    class GBC {
        public: 
            GBC();
            void start();
            void run();
            void start_window();
            void execute_cycle();
            void debug_execute_cycle(bool flag);
            void dump_stuff();
            inline void handle_input();

            address_bus addresses;

            SM83 cpu;
            PPU ppu;

            int prevpc = 0, cachedsp, frame = 0;
            unsigned long long cycle_count = 0;

    };
}