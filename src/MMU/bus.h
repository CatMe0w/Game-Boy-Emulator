#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <stdexcept>
#include <string>
#include <iostream>
#include <bitset>
#include <fstream>

namespace GBC {
    constexpr uint16_t BOOT_ROM = 0x0100;

    constexpr uint16_t ROM_BANK_00 = 0x0000;
    constexpr uint16_t END_ROM_BANK_00 = 0x3FFF;

    constexpr uint16_t ROM_BANK_NN = 0x4000;
    constexpr uint16_t END_ROM_BANK_NN = 0x7FFF;

    constexpr uint16_t VIDEO_RAM = 0x8000;
    constexpr uint16_t END_VIDEO_RAM = 0x9FFF;

    constexpr uint16_t EXTERNAL_RAM = 0xA000;
    constexpr uint16_t END_EXTERNAL_RAM = 0xBFFF;
    constexpr uint16_t WORK_RAM_BANK0 = 0xC000;
    constexpr uint16_t END_WORK_RAM_BANK0 = 0xCFFF;
    constexpr uint16_t WORK_RAM_BANKN = 0xD000;
    constexpr uint16_t END_WORK_RAM_BANKN = 0xDFFF;

    constexpr uint16_t ECHO_RAM1 = 0xE000;
    constexpr uint16_t EECHO_RAM1 = 0xEFFF;
    constexpr uint16_t ECHO_RAM2 = 0xF000;
    constexpr uint16_t EECHO_RAM2 = 0xFDFF;

    constexpr uint16_t SPRITEATTRIBUTETABLE = 0xFE00;
    constexpr uint16_t OAMaddress = 0xFE00;
    constexpr uint16_t END_OAM = 0XFE9F;
    constexpr uint16_t IO_REGISTERS = 0xFF00;
    constexpr uint16_t END_IO_REGISTERS = 0xFF7F;

    constexpr uint16_t NOT_USUABLE = 0xFFA0;
    constexpr uint16_t END_NOT_USUABLE = 0xFF9F;

    constexpr uint16_t JOYP = 0xFF00;

    constexpr uint16_t SB = 0xFF01;
    constexpr uint16_t SC = 0xFF02;

    constexpr uint16_t DIV = 0xFF04;
    constexpr uint16_t TIMA = 0xFF05;
    constexpr uint16_t TMA = 0xFF06;
    constexpr uint16_t TAC = 0xFF07;

    constexpr uint16_t NR10 = 0xFF10;
    constexpr uint16_t NR11 = 0xFF11;
    constexpr uint16_t NR12 = 0xFF12;
    constexpr uint16_t NR13 = 0xFF13;
    constexpr uint16_t NR14 = 0xFF14;
    constexpr uint16_t NR30 = 0xFF1A;
    constexpr uint16_t NR31 = 0xFF1B;
    constexpr uint16_t NR32 = 0xFF1C;
    constexpr uint16_t NR33 = 0xFF1D;
    constexpr uint16_t NR34 = 0xFF1E;
    constexpr uint16_t NR41 = 0xFF20;
    constexpr uint16_t NR42 = 0xFF21;
    constexpr uint16_t NR43 = 0xFF22;
    constexpr uint16_t NR44 = 0xFF23;
    constexpr uint16_t NR50 = 0xFF24;
    constexpr uint16_t NR51 = 0xFF25;
    constexpr uint16_t NR52 = 0xFF26;

    constexpr uint16_t WavePatternRAM = 0xFF30;
    // ends at FF3F 

    constexpr uint16_t LCDC = 0xFF40;
    constexpr uint16_t STAT = 0xFF41;
    constexpr uint16_t SCY = 0xFF42;
    constexpr uint16_t SCX = 0xFF43;
    constexpr uint16_t LYC = 0xFF45;
    constexpr uint16_t LY = 0xFF44;
    constexpr uint16_t OAMDMA = 0xFF46;
    constexpr uint16_t BGP = 0xFF47;
    constexpr uint16_t OBP0 = 0xFF48;
    constexpr uint16_t OBP1 = 0xFF49;
    constexpr uint16_t WY = 0xFF4A;
    constexpr uint16_t WX = 0xFF4B;

    // CGB
    constexpr uint16_t HDMA1 = 0xFF51;
    constexpr uint16_t HDMA2 = 0xFF52;
    constexpr uint16_t HDMA3 = 0xFF53;
    constexpr uint16_t HDMA4 = 0xFF54;
    constexpr uint16_t HDMA5 = 0xFF55;
    constexpr uint16_t VBK = 0xFF4F;
    constexpr uint16_t KEY1 = 0xFF4D;
    constexpr uint16_t RP = 0xFF56;
    constexpr uint16_t KEY0 = 0xFF4C;
    constexpr uint16_t OPRI = 0xFF6C;
    constexpr uint16_t SVBK = 0xFF70;
    // ALL

    constexpr uint16_t HIGH_RAM = 0xFF80;
    constexpr uint16_t END_HIGH_RAM = 0XFFFE;

    constexpr uint16_t IF = 0xFF0F;

    constexpr uint16_t IE = 0xFFFF;

    constexpr uint16_t KB = 1024;

    using byte = uint8_t;
    using half = uint16_t;

    enum state : uint8_t {
        hblank = 0,
        vblank = 1,
        OAMscan = 2,
        draw = 3
    };

    // should create more robust constructor
    struct address_bus { 
        byte bootrom[0xFF];
        byte cartROM[KB*KB];
        byte cartRAM[32*KB];
        byte workRAM[32 * KB]; 
        byte videoRAM[16 * KB];
        byte OAM[0x9F];
        byte IOrange[256];
        byte HRAM[128];
        byte IEnable = 0;
        byte RAMenable = 0;
        byte bankMode;
        state lcd_mode = hblank;
        byte latch_write = 0;
        byte rtc[4]{0};
        byte input_d = 0x0F;
        byte input_s = 0x0F;

        bool booting = 0;
        bool latched = 0;
        bool debug = 0;
        byte debug_value = 0;
        
        uint8_t mbc = 0;
        uint8_t vram_bank = 0;
        uint8_t wram_bank = 1;
        uint8_t rom_bank = 1;
        uint8_t eram_bank = 1;
        uint8_t bank_mode = 0;


        address_bus(); 

        void load_boot_ROM(const char* fname, uint32_t size);
        void load_ROM(const char* fname, uint32_t size);

        void load_RAM(const char* fname, uint32_t size);

        void write(half address, byte value);
        void writeMBC3(half address, byte value);
        void writeMBC1(half address, byte value);

        void writeIO(half address, byte val);

        uint8_t read(half address);
        uint8_t read_privledged(half address);
        uint8_t readIO(half address);
    };


}