#include "bus.h"

namespace GBC {
    class APU {
        void execute_cycle();
        byte div, div_prev;
    };
}

