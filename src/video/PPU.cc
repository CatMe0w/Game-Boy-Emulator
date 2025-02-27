#include "PPU.h"
#include "bus.h"
#include <algorithm>
#include <cstdint>
#include <fstream>



namespace GBC {
        void PPU::init_window() {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_CreateWindowAndRenderer("(GBC) hello window", WINDOW_WIDTH*4, WINDOW_HEIGHT*4, SDL_WINDOW_RESIZABLE, &window, &renderer);
        SDL_SetRenderScale(renderer, 4, 4);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
    }

    PPU::~PPU() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void PPU::execute_cycle() {
        lines %= 154;
        
        if ((bus->read(LCDC)&0x80) == 0) return;

        bus->IOrange[LY-IO_REGISTERS] = lines;

        bus->write(IF, bus->read(IF) & ~(1 << 1));

        if (lines < 144) {
            if ((dots%456) < 80) {
                if (mode != OAMscan) {
                    objnum = 0;
                    mode = OAMscan; 
                    byte objsize = (bus->read(LCDC) & (1 << 2)) ? 16 : 8;
                    
                    for (int16_t i = 0x00; i < 0x9F; i+=4) {
                        byte objy = bus->read(0xFE00+i)-16,
                        objx = bus->read(0xFE00+i+1)-1,
                        index = bus->read(0xFE00+i+2),
                        flags = bus->read(0xFE00+i+3); 
                        if (objy <= lines && objy+objsize > lines) {
                            objbuffer[objnum].objx = objx,
                            objbuffer[objnum].objy = objy,
                            objbuffer[objnum].index = index,
                            objbuffer[objnum].flags = flags;
                            ++objnum;
                            
                        }
                        
                        if (objnum == 10) break;
                    }
                    std::sort(objbuffer, objbuffer+objnum, [](obj a, obj b){return a.objx < b.objx;});
                }

                bus->IOrange[STAT-IO_REGISTERS] = (bus->IOrange[STAT-IO_REGISTERS] & 0xF8) | 2 | ((lines == bus->read(LYC)) << 2);  
                if (bus->read(STAT) & (1 << 5)) bus->write(IF, bus->read(IF) | (1 << 1));
            } else if ((dots%456) < 252) {
                if (mode != draw) {
                    mode = draw;
                    
                }
                bus->IOrange[STAT-IO_REGISTERS] = (bus->IOrange[STAT-IO_REGISTERS] & 0xF8) | 3 | ((lines == bus->read(LYC)) << 2);

                draw_pixel();
            } else {
                if (mode != hblank) {
                    mode = hblank;
                    renderX = 0;
                }
                bus->IOrange[STAT-IO_REGISTERS] = (bus->IOrange[STAT-IO_REGISTERS] & 0xF8) | 0 | ((lines == bus->read(LYC)) << 2);
                if (bus->read(STAT) & (1 << 3)) bus->write(IF, bus->read(IF) | (1 << 1));
            } 
        } else {
            if (mode != vblank) {
                mode = vblank;
                bus->write(IF, bus->read(IF) | 1);
                SDL_RenderPresent(renderer);
            }

            bus->IOrange[STAT-IO_REGISTERS] = (bus->IOrange[STAT-IO_REGISTERS] & 0xF8) | 1 | ((lines == bus->read(LYC)) << 2);
            if (bus->read(STAT) & (1 << 4)) bus->write(IF, bus->read(IF) | (1 << 1));
        }
        // bus->lcd_mode = mode; TODO, activate this, has been buggy so far

        if (bus->read(STAT) & (1 << 6) && (lines == bus->read(LYC))) bus->write(IF, bus->read(IF) | (1 << 1));

        if (dots >= 456) ++lines;
        dots %= 456;
        ++dots;
    }

    // TODO clean this up, and fix it
    void PPU::draw_pixel() {
        byte bgenable = bus->read(LCDC) & 1;
        byte objenable = bus->read(LCDC) & (1 << 1);
        byte windowenable = bus->read(LCDC) & (1 << 5);

        byte wx = bus->read(WX),
             wy = bus->read(WY);

        byte winbg = 0;
        byte objpix;

        if (bgenable) {
            if (windowenable && (((renderX+7 >= wx) && (lines >= wy)))){
                winbg = windowFIFO();
            } else {
                winbg = bgFIFO();
            }
        } else {
            winbg = 0;
        }

        byte pixel = 0;

        if (objenable) {
            objpix = objFIFO();
            if ((objpix > 64 || objpix == 0)) {
                pixel = winbg;
            } else {
                pixel = objpix;
            }
        } else {
            pixel = winbg;
        }
        
        
        SDL_SetRenderDrawColor(renderer, 220-(255.0/2)*(pixel), 255-(255.0/2)*(pixel), 220-(255.0/2)*(pixel), 255);

        SDL_RenderPoint(renderer,renderX++-6, lines);     
    }

    // TODO fix issues
    byte PPU::objFIFO() {
        byte tilei = 11;

        byte objectpix = 0;
        byte objsize = (bus->read(LCDC) & (1 << 2)) ? 16 : 8;
        
        uint8_t tilex = 0;
        for (int i = 0; i < objnum; ++i) {
            obj cand_tile = objbuffer[i];
            if (cand_tile.objx <= renderX && cand_tile.objx+8 > renderX && cand_tile.objx >= tilex) tilei = i;
        }

        if (tilei == 11) return 0;

        obj tile = objbuffer[tilei]; 

        byte objy = tile.objy,
            objx = tile.objx,
            flags = tile.flags,
            index = tile.index;
        

        if (objsize == 16) {
            index = (index & 0xFE);
        }
        
        bool palette = flags & (1 << 4),
                Xflip = flags & (1 << 5),
                Yflip = flags & (1 << 6),
                prio = flags & (1 << 7);


        half tile_address = 0x8000+index*16 + (Yflip ? (objsize-1-(lines-objy))*2 : (lines-objy)*2);

        byte tilelow = bus->read(tile_address),
            tilehigh = bus->read(tile_address+1);

        byte sampleobj = (((1 << (Xflip ? (renderX-objx) : 7-(renderX-objx))) & tilelow) != 0) | ((((1 << (Xflip ?  (renderX-objx) : 7-(renderX-objx))) & tilehigh) != 0) << 1);
        
        objectpix = sampleobj | prio | palette;

        return objectpix; 
    }

    byte PPU::bgFIFO() {
        byte bgenable = bus->read(LCDC) & 1;

        half BG_tile_map = (bus->read(LCDC) & (1 << 3)) ? 0x9C00 : 0x9800;
        byte data_area = (bus->read(LCDC) & (1 << 4));

        byte scx = bus->read(SCX),
             scy = bus->read(SCY);

        half tilex = (scx+renderX-7)%256, tiley = (scy+lines)%256;
        half tile_index_index = (tilex/8)+((tiley)/8)*32;

        byte tile_index = bus->read(tile_index_index+BG_tile_map);

        byte tilelow, tilehigh;

        if (data_area) {
            tilelow = bus->read(tile_index*16+(tiley%8)*2+0x8000);
            tilehigh = bus->read(tile_index*16+(tiley%8)*2+1+0x8000);
        } else {
            if (tile_index <= 127) {
                tilelow = bus->read((tiley%8)*2+0x9000+tile_index*16);
                tilehigh = bus->read((tiley%8)*2+1+0x9000+tile_index*16);
            } else {
                tilelow = bus->read((tiley%8)*2+0x8800+tile_index*16);
                tilehigh = bus->read((tiley%8)*2+1+0x8800+tile_index*16);
            }
        }

        byte samplepix = (((1 << (7-tilex%8)) & tilelow) != 0) | ((((1 << (7-tilex%8)) & tilehigh) != 0) << 1);
        return samplepix;
    }

    byte PPU::windowFIFO() {
        byte wx = bus->read(WX),
             wy = bus->read(WY);

        if ((renderX < wx-7) || (lines < wy)) return 0;
        byte windowEnable = (bus->read(LCDC) & (1 << 5));

        half w_tile_map = (bus->read(LCDC) & (1 << 6)) ? 0x9C00 : 0x9800;  
        byte data_area = (bus->read(LCDC) & (1 << 4));
        

        byte tilex = renderX-wx, tiley = lines-wy;

        half tile_index_index = (tilex/8)+((tiley)/8)*32;

        byte tile_index = bus->read(tile_index_index+w_tile_map);

        byte tilelow, tilehigh;

        if (data_area) {
            tilelow = bus->read(tile_index*16+(tiley%8)*2+0x8000);
            tilehigh = bus->read(tile_index*16+(tiley%8)*2+1+0x8000);
        } else {
            tilelow = bus->read((tiley%8)*2+0x8800-tile_index*16);
            tilehigh = bus->read((tiley%8)*2+1+0x8800-tile_index*16);
        }

        byte samplepix = (((1 << (7-tilex%8)) & tilelow) != 0) | ((((1 << (7-tilex%8)) & tilehigh) != 0) << 1);

        return samplepix;
    }

    void PPU::render_debug() {
        for (int i = 0; i < 382; ++i) {
            for (int j = 0; j < 8; ++j) {
                byte a = bus->read(0x8000+i*16+j*2), b = bus->read(0x8000+i*16+j*2+1);
                for (int k = 0; k < 8; ++k) {
                        int temp = (((1 << (7-k)) & a) != 0) | ((((1 << (7-k)) & b) != 0) << 1);
                        SDL_SetRenderDrawColor(renderer, 255-255.0/3*(temp), 255-255.0/3*(temp), 255-255.0/3*(temp), 255);
                        SDL_RenderPoint(renderer, (i%16)*8+k, int(i/16)*8+j);
                }
            }
        }
    }

    void PPU::dump_info() {
        std::cerr << std::hex << "dots: " << dots << '\n';
        std::cerr << "lines: " << lines << '\n';
        std::cerr << "renderX: " << renderX << '\n';
        switch(mode) { 
            case hblank:
            std::cerr << "state: hblank\n";
            break;
            case vblank:
            std::cerr << "state: vblank\n";
            break;
            case OAMscan:
            std::cerr << "state: OAMscan\n";
            break;
            case draw:
            std::cerr << "state: draw\n";
            break;
        }
        std::cerr << std::endl;
    }

    void PPU::dump_vram() {
        for(int i = 0; i < 0x3FF; ++i) {
            if (i%16 == 0) std::cout << std::endl;
            std::cout << std::hex << (i+0x9900) << ": " << std::bitset<8>(bus->read(i+0x9900) & 0xFF) << " ";
        }
    }
} 