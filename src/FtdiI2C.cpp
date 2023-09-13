#include <chrono>
#include <thread>
#include <vector>
#include "FtdiI2C.hpp"

#include "debuger.h"

using namespace std::chrono_literals;

static std::vector<FT_DEVICE_LIST_INFO_NODE> g_FT4222DevList;


FtdiI2C::FtdiI2C(uint8 slave_address, int device, int tries) : m_iSlaveAddress(slave_address), m_iDeviceNumber(device), m_iTries(tries) {
    if (g_FT4222DevList.empty()) {
        if (0 == FindDevices(false))
            throw FtdiException("No FTDI devices present");
    }

    FT_STATUS ftStatus;
    if (m_iDeviceNumber >= g_FT4222DevList.size())
        throw FtdiException("Device not found");

    ftStatus = FT_OpenEx((PVOID)g_FT4222DevList[m_iDeviceNumber].LocId, FT_OPEN_BY_LOCATION, &m_xHandle);
    if (FT_OK != ftStatus) {
        throw FtdiException("Cannot open device");
    }

    ftStatus = FT4222_I2CMaster_Init(m_xHandle, 100);
    if (FT_OK != ftStatus) {
        throw FtdiException("Cannot init IIC Master");
    }
}


FtdiI2C::~FtdiI2C() {
    if (m_xHandle) {
        FT4222_UnInitialize(m_xHandle);
        FT_Close(m_xHandle);
    }
}


FT4222_STATUS FtdiI2C::Read(uint16 slaveAddress, uint8 *buffer, uint16 bufferSize, uint16 &sizeTransferred) {
    return FT4222_I2CMaster_Read(m_xHandle, slaveAddress, buffer, bufferSize, &sizeTransferred);
}

FT4222_STATUS FtdiI2C::ReadEx(uint16 slaveAddress, I2CFlag flag, uint8 *buffer, uint16 bufferSize, uint16 &sizeTransferred) {
    return FT4222_I2CMaster_ReadEx(m_xHandle, slaveAddress, flag, buffer, bufferSize, &sizeTransferred);
}

FT4222_STATUS FtdiI2C::Write(uint16 slaveAddress, uint8 *buffer, uint16 bufferSize, uint16 &sizeTransferred) {
    return FT4222_I2CMaster_Write(m_xHandle, slaveAddress, buffer, bufferSize, &sizeTransferred);;
}

FT4222_STATUS FtdiI2C::WriteEx(uint16 slaveAddress, I2CFlag flag, uint8 *buffer, uint16 bufferSize, uint16 &sizeTransferred) {
    return FT4222_I2CMaster_WriteEx(m_xHandle, slaveAddress, flag, buffer, bufferSize, &sizeTransferred);
}

FT4222_STATUS FtdiI2C::GetStatus(uint8 &status) {
    return FT4222_I2CMaster_GetStatus(m_xHandle, &status);
}

FT4222_STATUS FtdiI2C::ResetBus() {
    return FT4222_I2CMaster_ResetBus(m_xHandle);
}

FT4222_STATUS FtdiI2C::Reset() {
    return FT4222_I2CMaster_Reset(m_xHandle);
}

static std::string DeviceFlagToString(DWORD flags) {
    std::string msg;
    msg += (flags & 0x1)? "DEVICE_OPEN" : "DEVICE_CLOSED";
    msg += ", ";
    msg += (flags & 0x2)? "High-speed USB" : "Full-speed USB";
    return msg;
}

int FtdiI2C::FindDevices(bool show) {
    FT_STATUS ftStatus = 0;
    DWORD numOfDevices = 0;
    FT_CreateDeviceInfoList(&numOfDevices);
    g_FT4222DevList.clear();

    for (DWORD iDev = 0; iDev < numOfDevices; ++iDev) {
        FT_DEVICE_LIST_INFO_NODE devInfo{0};
        ftStatus = FT_GetDeviceInfoDetail(iDev, &devInfo.Flags, &devInfo.Type, &devInfo.ID, &devInfo.LocId,
                                          devInfo.SerialNumber, devInfo.Description, &devInfo.ftHandle);
        if (FT_OK == ftStatus) {
            const std::string desc = devInfo.Description;
            if (desc == "FT4222" || desc == "FT4222 A") {
                g_FT4222DevList.push_back(devInfo);
                // if (show) {
                //     std::cout << "Dev : " << iDev << "\n";
                //     std::cout << "  Flags= 0x{:X}, ({})\n" << devInfo.Flags << DeviceFlagToString(devInfo.Flags);
                //     fmt::print("  Type= 0x{:X}\n", devInfo.Type);
                //     fmt::print("  ID= 0x{:X}\n", devInfo.ID);
                //     fmt::print("  LocID= 0x{:X}\n", devInfo.LocId);
                //     fmt::print("  SerialNumber= {}\n", devInfo.SerialNumber);
                //     fmt::print("  Description= {}\n\n", devInfo.Description);
                // }
            }
        }
    }
    return g_FT4222DevList.size();
}


bool FtdiI2C::WaitBusy(uint8 &status, int timeout) {
    status = 0;
    while (timeout > 0) {
        GetStatus(status);
        if (!I2CM_CONTROLLER_BUSY(status)) {
            break;
        }
        timeout--;
        std::this_thread::sleep_for(1ms);
    }
    return timeout > 0;
}


bool FtdiI2C::WriteLTCRegister16b(uint8 address, uint16 data) {
int tries = m_iTries;
FT4222_STATUS ret;
uint16 written;
uint8 status = 0;
std::vector<uint8> tx_vector;
    tx_vector.push_back(address);
    tx_vector.push_back(data & 0xFF);
    tx_vector.push_back(data >> 8);    

    while (tries > 0) {
        print_debug_info("Try to send data on LTC\n");
        ret = WriteEx(m_iSlaveAddress, FLAG_START_AND_STOP, tx_vector.data(), tx_vector.size(), written);
        if ((ret == FT4222_OK) && (written == tx_vector.size())) {   
            print_debug_info("Data have been sent\n");         
            if (WaitBusy(status, 50)) {
                GetStatus(status);
                if (!(I2CM_DATA_NACK(status) || I2CM_ADDRESS_NACK(status) || I2CM_ARB_LOST(status) || I2CM_BUS_BUSY(status) || I2CM_CONTROLLER_BUSY(status))) {
                    print_debug_info("Controller is now available\n");
                    break;
                } else {
                    print_debug_info("Error occured during writing operation\n");
                    PrintStatus(status);
                }
            }
        }
        if (tries > 0) {
            Reset();
            tries--;
            std::this_thread::sleep_for(std::chrono::milliseconds(4));
            continue;
        }
    }
    if (tries == 0) {
        throw FtdiException("Cannot write register");
    }
    return true;
}

uint16 FtdiI2C::ReadLTCRegister16b(uint8 address) {
int tries = m_iTries;
uint16 read_value = 0;
FT4222_STATUS ret;
uint16 transferred;
uint8 status;

    while (tries > 0) {
        ret = WriteEx(m_iSlaveAddress, FLAG_START, &address, sizeof(address), transferred);
        print_debug_info("Try to send command on LTC\n");
        if ((ret == FT4222_OK) && (transferred == sizeof(address))) {
            GetStatus(status);
            ret = GetStatus(status);
            if (!((ret != FT4222_OK) || I2CM_ADDRESS_NACK(status) || I2CM_DATA_NACK(status))) {
                print_debug_info("Try to recieve data from LTC\n");
                std::vector<uint8> readed;
                readed.resize(2, 0);
                ret = ReadEx(m_iSlaveAddress, FLAG_START_AND_STOP, readed.data(), readed.size(), transferred);
                if ((ret == FT4222_OK) && (transferred == readed.size())) {
                    GetStatus(status);
                    if (!(I2CM_DATA_NACK(status) || I2CM_ADDRESS_NACK(status) || I2CM_ARB_LOST(status) || I2CM_BUS_BUSY(status) || I2CM_CONTROLLER_BUSY(status))) {
                            read_value = (readed[1] << 8) | readed[0];
                            print_debug_info("Data sucessfully recieved\n");
                            break;
                    }
                }
            }
        }
        if (tries > 0) {
            Reset();
            tries--;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
    }
    if (tries == 0)
        throw FtdiException("Cannot read register");

    return read_value;
}


bool FtdiI2C::Write24LC00Register8b(uint8 deviceadr, uint8 address, uint8 data) {
int tries = m_iTries;
FT4222_STATUS ret;
uint16 written;
uint8 status = 0;
std::vector<uint8> tx_vector;
    tx_vector.push_back(address);
    tx_vector.push_back(data);

    while (tries > 0) {
        print_debug_info("Try to send data on 24LC00 EEPROM\n");
        ret = WriteEx(deviceadr, FLAG_START_AND_STOP, tx_vector.data(), tx_vector.size(), written);
        if ((ret == FT4222_OK) && (written == tx_vector.size())) { 
            print_debug_info("Data have been sent\n");           
            if (WaitBusy(status, 50)) {
                GetStatus(status);
                if (!(I2CM_DATA_NACK(status) || I2CM_ADDRESS_NACK(status) || I2CM_ARB_LOST(status) || I2CM_BUS_BUSY(status) || I2CM_CONTROLLER_BUSY(status))) {
                    print_debug_info("Controller is now available\n");
                    break;
                } else {
                    print_debug_info("Error occured during writing operation\n");
                    PrintStatus(status);
                }
            }
        }
        if (tries > 0) {
            Reset();
            //ResetBus();
            tries--;
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
            continue;
        }
    }
    if (tries == 0) {
        throw EEPROMException("Cannot write register");
    }
    return true;
}


uint8 FtdiI2C::Read24LC00Register8b(uint8 deviceadr, uint8 address) {
int tries = m_iTries;
uint8 read_value = 0;
FT4222_STATUS ret;
uint16 transferred;
uint8 status;

    while (tries > 0) {
        print_debug_info("Try to send command on 24LC00 EEPROM\n");
        ret = WriteEx(deviceadr, FLAG_START, &address, sizeof(address), transferred);
        if ((ret == FT4222_OK) && (transferred == sizeof(address))) {
            GetStatus(status);
            ret = GetStatus(status);
            print_debug_info("Try to recieve data from 24LC00 EEPROM\n");
            if (!((ret != FT4222_OK) || I2CM_ADDRESS_NACK(status) || I2CM_DATA_NACK(status))) {
                ret = ReadEx(deviceadr, FLAG_START_AND_STOP, &read_value, 1, transferred);
                if ((ret == FT4222_OK) && (transferred == 1)) {
                    GetStatus(status);
                    if (!(I2CM_DATA_NACK(status) || I2CM_ADDRESS_NACK(status) || I2CM_ARB_LOST(status) || I2CM_BUS_BUSY(status) || I2CM_CONTROLLER_BUSY(status))) {
                        print_debug_info("Data sucessfully recieved\n");
                        break;
                    }
                }
            }
        }
        if (tries > 0) {
            Reset();
            tries--;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
    }
    if (tries == 0)
        throw EEPROMException("Cannot read register");

    return read_value;
}