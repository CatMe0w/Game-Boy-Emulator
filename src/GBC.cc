#include "GBC.h"
#include "bus.h"
#include <exception>
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace GBC {
    GBC::GBC() : cpu(&addresses), ppu(&addresses) {
        start();
        std::ofstream("log.txt") << "";
    } 

    void GBC::start() {
        ppu.init_window();
        addresses.booting = false;
        addresses.IOrange[JOYP-IO_REGISTERS] = 0xFF;
    }

    // This function is a mess, I'm using it to debug stuff right now
    void GBC::run() {
        if (addresses.booting == 0) {
            cpu.RA = 0x01;
            cpu.RB = 0xFF;
            cpu.RC = 0x13;
            cpu.RD = 0x00;
            cpu.RE = 0xC1;
            cpu.RH = 0x84;
            cpu.RL = 0x03;
            cpu.pc = 0x100;
            cpu.sp = 0xFFFE;
        } else {
            cpu.pc = 0;
        }
        bool breakflag = 0;
        while(true) {
            ++frame;

            // frame, TODO move out into function
            for (int i = 0; i < 70224; ++i) {
                debug_execute_cycle(0);

                if (ppu.mode == vblank) {
                    for (int j = 0; j < 4560; ++j) {
                        debug_execute_cycle(0);
                    }
                    break;
                }
            } 

            while (SDL_PollEvent(&ppu.event)) {
                byte input_s = 0, input_d = 0;
                if (ppu.event.type == SDL_EVENT_QUIT) breakflag = 1;
            }

            handle_input();

            if (breakflag) break;
            std::this_thread::sleep_for(16ms); // TODO make sleep based on elapsed frame execution time
        }
    }

    void GBC::handle_input() {
        byte input_s = 0xF0, input_d = 0xF0;

        if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_A]) {
            input_s |= 1;
        }
        if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_S]) {
            input_s |= (1 << 1);
        }
        if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_Z]) {
            input_s |= (1 << 2);
        }
        if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_X]) {
            input_s |= (1 << 3);
        }

        if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RIGHT]) {
            input_d |= 1;
        }
        if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LEFT]) {
            input_d |= (1 << 1);
        }
        if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_UP]) {
            input_d |= (1 << 2);
        }
        if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_DOWN]) {
            input_d |= (1 << 3);
        }

        if ((addresses.read_privledged(JOYP) & (3 << 4)) == 0) {
            if ((input_d != addresses.input_d) | (input_s != addresses.input_s)) {
                addresses.write(IF, addresses.read(IF) | (1 << 4));
            }
        } else if ((addresses.read_privledged(JOYP) & (1 << 4)) == 0) {
            if ((input_d != addresses.input_d)) {
                addresses.write(IF, addresses.read(IF) | (1 << 4));
            }
        } else if ((addresses.read_privledged(JOYP) & (2 << 4)) == 0) {
            if ((input_s != addresses.input_s)) {
                addresses.write(IF, addresses.read(IF) | (1 << 4));
            }
        }


        addresses.input_d = ~input_d;
        addresses.input_s = ~input_s;
    }

    void GBC::execute_cycle() {
        cpu.execute();
        ppu.execute_cycle();
        ++cycle_count;
    }

    void GBC::debug_execute_cycle(bool flag) {
        prevpc = cpu.pc;
        execute_cycle();
        
        if (cycle_count % 10000 == 0) { 
            // std::cout << std::dec << cycle_count << std::hex << std::endl; 
            dump_stuff();
        }
    }

    void GBC::dump_stuff() {
        std::ofstream("log.txt", std::ofstream::app) 
        << "_________\n"
        << "cycles: " << cycle_count << '\n'
        << "frame: " << frame << '\n'
        << "pc: " << std::hex << cpu.pc << '\n'
        << "cycles: " << std::hex << (int)cpu.cycles << '\n'
        << "instr: " << std::hex << (int)addresses.read(cpu.pc) << '\n'
        << "rom bank: " << std::hex << (int)addresses.rom_bank << '\n'
        << "sp: " << std::hex << cpu.sp << '\n'
        << "top of stack: " << std::hex << ((uint16_t)addresses.read(cpu.sp) | 
         ((uint16_t)addresses.read(cpu.sp+1) << 8)) << '\n'
        << "flags: " << std::bitset<8>(cpu.RF) << '\n'
        << "HL: " << std::hex << cpu.getHL() << '\n'
        << "stat: " << std::bitset<8>(addresses.read(STAT)) << '\n'
        << "LCDC: " << std::bitset<8>(addresses.read(LCDC)) << '\n'
        << "lyc: " << std::hex << (int)addresses.read(LCDC) << '\n'
        << "IE: " << std::hex << (int)addresses.read(IE) << '\n'
        << "IF: " << std::hex << (int)addresses.read(IF) << '\n'
        << "IME: " << std::hex << (int)cpu.IME << '\n'
        << "bg tile map: " << std::hex << (addresses.read(LCDC) & (1 << 3) ? 0x9C00 : 0x9800) << '\n'
        << std::hex << "dots: " << ppu.dots << '\n'
        << "lines: " << ppu.lines << '\n'
        << "renderX: " << ppu.renderX << '\n';
        switch(ppu.mode) { 
            case hblank:
            std::ofstream("log.txt", std::ofstream::app) << "state: hblank\n";
            break;
            case vblank:
            std::ofstream("log.txt", std::ofstream::app) << "state: vblank\n";
            break;
            case OAMscan:
            std::ofstream("log.txt", std::ofstream::app) << "state: OAMscan\n";
            break;
            case draw:
            std::ofstream("log.txt", std::ofstream::app) << "state: draw\n";
            break;
        }
        std::ofstream("log.txt", std::ofstream::app) << std::endl;
        
        std::ofstream("log.txt", std::ofstream::app) << "LY: " << (int)ppu.bus->read(0xFF44) << '\n' << "_________" << std::endl;
        ppu.dump_info();
        cpu.dump_info();
    }
}
