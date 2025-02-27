#include "bus.h"

namespace GBC {
    class MMU {
        public:
        MMU(address_bus *memory) : memory(memory) { }
        address_bus *memory;

        void execute_cycle() {
            
        }
    };
}