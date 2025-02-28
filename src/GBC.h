#pragma once 


#include <stdlib.h>
#include <assert.h>
#include <cstdint>
#include <cstring>

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <bitset>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <filesystem>

#include "CPU/SM83.h"
#include "CPU/cycles.h"

#include "MMU/bus.h"
#include "video/PPU.h"
#include "bus.h"
#include <thread>


#define RES_X 160
#define RES_Y 144
#define COLORS 32768

using namespace std::chrono_literals;

using byte = uint8_t;
using half = uint16_t;

namespace GBC {
    class GBC {
        public: 
            GBC();
            void start();
            void run();
            void start_window();
            void execute_cycle();
            void debug_execute_cycle(bool flag);
            void dump_stuff();
            void handle_input();

            address_bus addresses;

            SM83 cpu;
            PPU ppu;

            int prevpc = 0, cachedsp, frame = 0;
            unsigned long long cycle_count = 0;

    };
}