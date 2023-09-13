#include "debuger.h"
#include <iostream>
#include <string>
#include <cstdint>

void print_debug_info(std::string str) {
    #ifdef SHOW_DEBUG_INFO
        std::cout << "[DEBUG] " << str;
    #endif
}

void PrintStatus(uint8_t status) {
    #ifdef SHOW_DEBUG_INFO
        std::cout << "[DEBUG] Status:\n";
        if (status & (1 << 0)) {
            std::cout << "    Controller busy\n";
        }
        if (status & (1 << 1)) {
            std::cout << "    Error condition\n";
        }
        if (status & (1 << 2)) {
            std::cout << "    Slave not ACK during last operation\n";
        }
        if (status & (1 << 3)) {
            std::cout << "    Data not ACK during last operation\n";
        }
        if (status & (1 << 4)) {
            std::cout << "    Arbitration LOST\n";
        }
        if (status & (1 << 5)) {
            std::cout << "    Controller IDLE\n";
        }
        if (status & (1 << 6)) {
            std::cout << "    BUS busy\n";
        }
    #endif
}