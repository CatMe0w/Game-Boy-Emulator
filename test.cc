#include "GBC.h"
#include <cmath>
#include <cstdint>
#include <fstream>

int main() {
    GBC::GBC gbc;

    // gbc.addresses.load_boot_ROM("dmg_boot.bin", 0xFF);
    gbc.addresses.load_ROM("dmg-acid2.gb", GBC::KB*GBC::KB);

    gbc.run();
}