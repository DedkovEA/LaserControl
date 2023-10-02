#include <iostream>
#include <thread>
#include <chrono>

#include <FtdiI2C.hpp>
#include "debuger.h"
#include "config.h"
#include "LTC5100Parameters.hpp"

#include <string>
#include <vector>
#include <cmath>
#include <limits>



enum RETURN_STATUS {
    OK = 0,
    FTDI_ERROR = 1,
    EEPROM_ERROR = 2
};

constexpr auto max_size = 100000;


void reprogram_EEPROM(FtdiI2C &i2c, uint8 eepromadr=0x50) {
    i2c.Write24LC00Register8b(eepromadr, 0x00, 0x97);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));  // maximum write cycle time for 24LC00 is 4ms
    i2c.Write24LC00Register8b(eepromadr, 0x01, 0x7f);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x02, 0xe7);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x03, 0x04);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x04, 0x00);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x05, 0x00);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x06, 0x00);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x07, 0x00);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x08, 0x00);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x09, 0x00);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x0A, 0x00);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x0B, 0x00);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x0C, 0x00);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x0D, 0x00);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x0E, 0x21);
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    i2c.Write24LC00Register8b(eepromadr, 0x0F, 0x10);
}


int main(int argc, char *argv[]) {

    int devnum = 0;                     // number of FTDI devices, no need to set it 
    uint8 slaveAddress = 0x0A;          // address of LTC5100 laser diod driver
    int tries = 2;                      // number of tries to send command
    bool need_programm_eeprom = true;  
    uint8 eepromadr = 0x50;             // address of EEPROM on common I2C bus

    std::cout << "===================\n";
    std::cout << "LaserControl v. " << LaserControl_VERSION_MAJOR << "." << LaserControl_VERSION_MINOR << "\n";
    std::cout << "===================\n";
    std::cout << "This program operates with VCSEL controlled by LTC5100 driver.\n";
    std::cout << "Type \'help\' for list of available commands.\n";

    try {
        FtdiI2C i2c(slaveAddress, devnum, tries);


        LTC5100Parameters controllerParams;
        std::vector<uint8> memory;
        memory.resize(16, 0);
        bool res = i2c.Read24LC00Sequential(eepromadr, 0x00, memory.data(), 16);
        if (res) {
            controllerParams.update_from_EEPROM(memory.data());
        } else {
            std::cout << "Something goes wrong, EEPROM was not properly read.";
        }

        std::string input = "";
        while (true) {
            //std::cin >> input;
            std::getline(std::cin, input);
            if (input == "H" || input == "help" || input == "h") {
                std::cout << "This program operates with VCSEL controlled by LTC5100 driver. Following commands are allowed:\n";
                std::cout << "    H(help, h)                --- display this message;\n";
                std::cout << "    SB(set bias, sb)          --- enter set bias current mode,\n";
                std::cout << "                                  in order to exit - send \'E\';\n";
                std::cout << "    R(reprogram, r)           --- repogram EEPROM;\n";
                std::cout << "    S(status, s)              --- enter status menu;\n";
                std::cout << "    DW(direct write, dw)      --- write into LTC5100 directy;\n";
                std::cout << "    E(exit, e)                --- exit program.\n";
            } else if (input == "R" || input == "reprogram" || input == "r") {
                reprogram_EEPROM(i2c, eepromadr);
            } else if (input == "SB" || input == "set bias" || input == "sb") {
                std::cout << "Choose bias setup mode:\n";
                std::cout << "   0 --- set bias range and bias current manually;\n";
                std::cout << "   1 --- set bias current in mA with automatic determination of bias current range.\n";
                int mode = 0;
                std::cin >> mode;
                if (std::cin.fail()) {
                    std::cout << "Invalid mode! Supported modes have been listed above. Enter \'SB\' once more in order to repeat mode selection.\n";
                    std::cin.clear();
                    std::cin.ignore(max_size, '\n');
                    continue;
                }
                std::cin.ignore(max_size, '\n');
                if (mode > 1) {
                    std::cout << "Invalid mode! Supported modes have been listed above. Enter \'SB\' once more in order to repeat mode selection.\n";
                    continue;
                }  

                // enter set_bias cycle
                std::string bias_in = "";
                while (true) {
                    std::getline(std::cin, bias_in);
                    //std::cin >> bias_in;
                    if (bias_in == "E" || bias_in == "exit" || bias_in == "e") {
                        std::cout << "Exit from bias setup mode\n";
                        break;
                    }
                    if (mode == 0) {
                        int delimpos = bias_in.find(' ');
                        if (delimpos == std::string::npos) {
                            std::cout << "Invalid arguments. Allowed usage are:\n";
                            std::cout << "    <bias_range [0, 3]> <bias_current [0, 1023]>\n";
                            std::cout << "where bias range determines maximum bias current at <bias_current == 1023> as (<bias_range>+1)*9mA.\n";
                            continue;
                        }
                        uint16 bias_range = 0;
                        uint16 bias_current = 0;
                        try {
                            bias_range = std::stoul(bias_in.substr(0, delimpos));
                            bias_current = std::stoul(bias_in.substr(delimpos+1));
                        } catch (const std::invalid_argument &e) {
                            std::cout << "Invalid argument! Error in interpreting values:\n";
                            std::cout << "    " << e.what() << "\n";
                            continue;
                        }

                        if (bias_range > 3) {
                            std::cout << "Invalid argument! Bias range must be <= 3.\n";
                            continue;
                        }
                        if (bias_current > 1023) {
                            std::cout << "Invalid argument! Bias current must be <= 1023.\n";
                            continue;
                        }
                        uint16 data2send = (bias_current | (bias_range << 10)) & 0x0fff;
                        std::cout << "Setting up bias current Ib = " << 9*(1+bias_range)*(float(bias_current)/1024) << " mA\n";
                        print_debug_info("Command on I2C line is: " + std::to_string(data2send) + "\n");
                        i2c.WriteLTCRegister16b(0x15, data2send);

                    } else if (mode == 1) {
                        float current = 0.;
                        try {
                            current = std::stof(bias_in);   
                        }
                        catch(const std::invalid_argument& e) {
                            std::cout << "Invalid argument! Error in interpreting value:\n";
                            std::cout << "    " << e.what() << '\n';
                            continue;
                        }
                        
                        
                        if (current < 0. || current > 36.) {
                            std::cout << "Invalid range of bias current! Ib must be in range from 0.0 to 36.0\n";
                            continue;
                        }

                        uint16 bias_range = std::floor((current + 0.05) / 9.);  // should be as low as possible, but still need some space for temperature compensation
                        bias_range = std::min<uint16>(bias_range, 3u);
                        uint16 bias_current = std::floor(current / 9. / (bias_range + 1) * 1024);
                        bias_current = std::min<uint16>(bias_current, 1023u);
                        uint16 data2send = (bias_current | (bias_range << 10)) & 0x0fff;
                        std::cout << "Setting up bias current Ib = [r:" << bias_range << ", c:" << bias_current << " = " << 9*(1+bias_range)*(float(bias_current)/1024) << " mA\n";
                        print_debug_info("Command on I2C line is: " + std::to_string(data2send) + "\n");
                        i2c.WriteLTCRegister16b(0x15, data2send);
                    }
                }

            } else if (input == "S" || input == "status" || input == "s") {
                std::cout << "Choose action on status:\n";
                std::cout << "   ed --- EEPROM dump;\n";
                std::cout << "   cd --- LTC5100 controller dump;\n";
                std::cout << "   s  --- Show current status;\n";
                std::cout << "   w  --- Write current status into EEPROM;\n";
                std::getline(std::cin, input);
                if(input == "ed" || input == "eeprom dump" || input == "ED") {
                    std::vector<uint8> memory;
                    memory.resize(16, 0);
                    bool res = i2c.Read24LC00Sequential(eepromadr, 0x00, memory.data(), 16);
                    if (res) {
                        controllerParams.update_from_EEPROM(memory.data());
                        controllerParams.pretty_print();
                    } else {
                        std::cout << "Something goes wrong, EEPROM was not properly read.";
                    }
                } else if (input == "cd" || input == "controller dump" || input == "CD") {
                    controllerParams.update_from_controller(&i2c);
                    controllerParams.pretty_print(true);
                } else if (input == "s" || input == "show" || input == "S") {
                    controllerParams.pretty_print(true);
                } else if (input == "w" || input == "write" || input == "W") {
                    controllerParams.write_to_EEPROM(&i2c, eepromadr);
                } else {
                    std::cout << "Invalid action. Choose from actions listed above.\n";
                };
            } else if (input == "DW" || input == "direct write" || input == "dw") {
                std::cout << "You invoke direct write into LTC5100 controller. You are acting at your own\'s risk.\n";
                std::cout << "Usage:\n";
                std::cout << "   <LTC command in hex> <2 bytes to write in hex>\n";
                
                uint8_t command;
                uint16_t data;
                std::string write_input;
                std::getline(std::cin, write_input);

                int delimpos = write_input.find(' ');
                if (delimpos == std::string::npos) {
                    std::cout << "Invalid arguments. Allowed usage are:\n";
                    std::cout << "   <LTC command in hex> <2 bytes to write in hex>\n";
                    continue;
                }
                try {
                    command = std::stoul(write_input.substr(0, delimpos), nullptr, 16);
                    data = std::stoul(write_input.substr(delimpos+1), nullptr, 16);
                } catch (const std::invalid_argument &e) {
                    std::cout << "Invalid argument! Error in interpreting values:\n";
                    std::cout << "    " << e.what() << "\n";
                    continue;
                }

                std::cout << std::hex << command << "\n";
                std::cout << std::hex << data << "\n";
                print_debug_info("Writing command = " + std::to_string(command) + ", data = " + std::to_string(data) + "\n");
                i2c.WriteLTCRegister16b(command, data);
            } else if (input == "E" || input == "exit" || input == "e") {
                std::cout << "Exit from program";
                i2c.WriteLTCRegister16b(0x15, 0x00);
                break;
            } else {
                std::cout << "Invalid command. Type \'H\' for help.\n";
            }
        }

        // uint16 bias = 0;
        // uint16 bias_range = 0;
        // while (bias_range < 4) {
        //     std::cin >> bias_range >> bias;
        //     if (bias_range > 0x03) break;
        //     if (bias > 0x3ff) break;
        //     uint16 data2send = (bias | (bias_range << 10)) & 0x0fff;
        //     std::cout << "Setting up bias current Ib = " << 9*(1+bias_range)*(float(bias)/1024) << " mA\n";
        //     std::cout << "Command on I2C line is: " << data2send << "\n";
        //     i2c.WriteLTCRegister16b(0x15, data2send);
        // }

        return RETURN_STATUS::OK;

    } catch (const FtdiException &e) {
        std::cout << "FTDI error: " << e.what() << "\n";
        return RETURN_STATUS::FTDI_ERROR;
    } catch (const EEPROMException &e) {
        std::cout << "Error: " << e.what() << "\n";
        return RETURN_STATUS::EEPROM_ERROR;
    }

    return RETURN_STATUS::OK;
}
