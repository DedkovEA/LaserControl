# LaserControl
Simple C++ program for control i2c VCSEL setup via FT4222H

## Installation
1. Download FTDI D2XX driver. For example, from: https://ftdichip.com/drivers/d2xx-drivers/
2. Download FTDI libraries: ftd2xx and LibFT4222. For example from: https://ftdichip.com/wp-content/uploads/2022/06/LibFT4222-v1.4.5.zip 
3. Put them into project root folder. Folder structure should be:
   
  .
  
  ├── src/
  
  │   └── .cpp files

  ├── include/
  │   └── .hpp and .h files
  ├── ftdi/
  │   ├── ftd2xx/
  │   │   ├── amd64/
  │   │   │   └── ftd2xx.lib
  │   │   ├── i386/
  │   │   │   └── ftd2xx.lib
  │   │   └── ftd2xx.h
  │   └── LibFT4222/
  │       ├── dll/
  │       │   ├── amd64/
  │       │   │   ├── LibFT4222-64.dll
  │       │   │   └── LibFT4222-64.lib
  │       │   ├── i386/
  │       │   │   ├── LibFT4222.dll
  │       │   │   └── LibFT4222.lib
  │       │   ├── LibFT4222.dll
  │       │   └── LibFT4222.lib
  │       ├── inc/
  │       │   └── ...
  │       └── lib/
  │           └── ...
  ├── CMakeLists.txt
  └── README.md

  You simply need to download library, create "ftdi" folder and unpack it there, taking tree structure listed above into account.
4. Build with cmake. You can set SHOW_DEBUG_INFO = ON option in order to see some debug messages.

## Possible issues
If errors occures during transmission try several more times.
Build tested on windows both on MinGW and Visual Studio 2019 compiler. 
