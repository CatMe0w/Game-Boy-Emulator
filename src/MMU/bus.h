#pragma once


#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <iostream>
#include <bitset>
#include <fstream>

#define ROM_BANK_00 0x0000
#define END_ROM_BANK_00 0x3FFF

#define ROM_BANK_NN 0x4000
#define END_ROM_BANK_NN 0x7FFF

#define VIDEO_RAM 0x8000
#define END_VIDEO_RAM 0x9FFF

#define EXTERNAL_RAM 0xA000
#define END_EXTERNAL_RAM 0xBFFF
#define WORK_RAM_BANK0 0xC000
#define END_WORK_RAM_BANK0 0xCFFF
#define WORK_RAM_BANKN 0xD000
#define END_WORK_RAM_BANKN 0xDFFF

#define ECHO_RAM1 0xE000
#define EECHO_RAM1 0xEFFF
#define ECHO_RAM2 0xF000
#define EECHO_RAM2 0xFDFF

#define SPRITEATTRIBUTETABLE 0xFE00
#define OAMaddress 0xFE00
#define END_OAM 0XFE9F
#define IO_REGISTERS 0xFF00
#define END_IO_REGISTERS 0xFF7F



#define JOYP 0xFF00

#define SB 0xFF01
#define SC 0xFF02

#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TAC 0xFF07


#define NR10 0xFF10
#define NR11 0xFF11
#define NR12 0xFF12
#define NR13 0xFF13
#define NR14 0xFF14
#define NR30 0xFF1A
#define NR31 0xFF1B
#define NR32 0xFF1C
#define NR33 0xFF1D
#define NR34 0xFF1E
#define NR41 0xFF20
#define NR42 0xFF21
#define NR43 0xFF22
#define NR44 0xFF23
#define NR50 0xFF24
#define NR51 0xFF25
#define NR52 0xFF26

#define LCDC 0xFF40
#define STAT 0xFF41
#define SCY 0xFF42
#define SCX 0xFF43
#define LYC 0xFF45
#define LCDY LY
#define LY 0xFF44
#define OAMDMA 0xFF46
#define BGP 0xFF47
#define OBP1 0xFF48
#define OBP2 0x49
#define WY 0xFF4A
#define WX 0xFF4B

// CGB
#define HDMA1 0xFF51
#define HDMA2 0xFF52
#define HDMA3 0xFF53
#define HDMA4 0xFF54
#define HDMA5 0xFF55
#define VBK 0xFF4F
#define KEY1 0xFF4D
#define RP 0xFF56
#define KEY0 0xFF4C
#define OPRI 0xFF6C
#define SVBK 0xFF70

// ALL

#define HIGH_RAM 0xFF80
#define END_HIGH_RAM 0XFFFE

#define IF 0xFF0F

#define IE IENABLE
#define IENABLE 0xFFFF


using byte = uint8_t;
using half = uint16_t;

#define KB 1024

namespace GBC {
    enum state : uint8_t {
        hblank = 0,
        vblank = 1,
        OAMscan = 2,
        draw = 3
    };

    struct address_bus { // this is probably very slow and inefficient
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
        


        address_bus(); 

        void load_boot_ROM(const char* fname, uint32_t size);
        void load_ROM(const char* fname, uint32_t size);

        void load_RAM(const char* fname, uint32_t size);

        void write(half address, byte value);
        void writeMBC3(half address, byte value);
        void writeIO(half address, byte val);

        uint8_t read(half address);
        uint8_t read_privledged(half address);
        uint8_t readIO(half address);

    };


}