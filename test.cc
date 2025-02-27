#include "GBC.h"
#include <cmath>
#include <cstdint>
#include <fstream>

int main() {
    GBC::GBC gbc;


    gbc.cpu.sp = 0xFFFE;
    gbc.addresses.booting = false;
    // gbc.addresses.load_boot_ROM("dmg_boot.bin", 0xFF);
    gbc.addresses.load_ROM("dmg-acid2.gb", KB*KB);

    for (int i = 0; i <= 0x39; ++i) {
        
    std::ofstream("log.txt", std::ofstream::app) << std::hex << "rom bank: "<< i << " address: "<< (int)gbc.addresses.read(0x6000) <<'\n';
    }

    

    gbc.run();



}