#pragma once
#include <Windows.h>
#include <ftd2xx.h>
#include <LibFT4222.h>
#include <exception>


enum I2CFlag {
    FLAG_NONE = 0x80,
    FLAG_START = 0x02,
    FLAG_REPEATED_START = 0x03,
    FLAG_STOP = 0x04,
    FLAG_START_AND_STOP = 0x06
};

class FtdiI2C {
public:
    FtdiI2C(uint8 slave_address, int device = 0, int tries = 1);
    ~FtdiI2C();

    static int FindDevices(bool show = false);
    FT4222_STATUS Write(uint16 slaveAddress, uint8 *buffer, uint16 bufferSize, uint16 &sizeTransferred);
    FT4222_STATUS WriteEx(uint16 slaveAddress, I2CFlag flag, uint8 *buffer, uint16 bufferSize, uint16 &sizeTransferred);
    FT4222_STATUS Read(uint16 slaveAddress, uint8 *buffer, uint16 bufferSize, uint16 &sizeTransferred);
    FT4222_STATUS ReadEx(uint16 slaveAddress, I2CFlag flag, uint8 *buffer, uint16 bufferSize, uint16 &sizeTransferred);
    FT4222_STATUS GetStatus(uint8 &status);
    FT4222_STATUS ResetBus();
    FT4222_STATUS Reset();
    
    bool WriteLTCRegister16b(uint8 address, uint16 data);
    bool Write24LC00Register8b(uint8 deviceadr, uint8 address, uint8 data);
    uint16 ReadLTCRegister16b(uint8 address);
    uint8 Read24LC00Register8b(uint8 deviceadr, uint8 address);

    bool WaitBusy(uint8 &status, int timeout = 20);

private:
    int m_iTries;
    uint8 m_iSlaveAddress;
    int m_iDeviceNumber = 0;
    FT_HANDLE m_xHandle {};
};

class FtdiException : public std::exception {
public:
    explicit FtdiException(const char *message) : msg(message) {}
    const char *what() const noexcept override {
        return msg;
    }

private:
    const char *msg;
};

class EEPROMException : public std::exception {
public:
    explicit EEPROMException(const char *message) : msg(message) {}
    const char *what() const noexcept override {
        return msg;
    }

private:
    const char *msg;
};


