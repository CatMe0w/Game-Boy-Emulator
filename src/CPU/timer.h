#include "bus.h"

// unfinished/moved to cpu

namespace GBC {
    class timer {
        address_bus *addresses;
        
        void execute_cycle();

        uint32_t ticker = 0;
    };
}