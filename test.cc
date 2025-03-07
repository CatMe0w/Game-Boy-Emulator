#include "GBC.h"
#include <cmath>
#include <cstdint>
#include <fstream>

int main() {
    GBC::GBC gbc;

    std::string path;
    std::cout << "enter path to rom: " << '\n';

    std::cin >> path;
    gbc.addresses.load_ROM(path.c_str(), GBC::KB*GBC::KB);

    gbc.run();
}