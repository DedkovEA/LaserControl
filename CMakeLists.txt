cmake_minimum_required(VERSION 3.10)

project(LaserControl VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)


option(SHOW_DEBUG_INFO "Whether to display some debug info" OFF)


#if (WIN32)
#    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
#        set(FT4222_DLL "${PROJECT_SOURCE_DIR}/lib/LibFT4222.dll")
#        set(FT4222_DLL_POSTFIX "")
#        link_directories(${PROJECT_SOURCE_DIR}/lib)
#    else()
#        set(FT4222_DLL "${PROJECT_SOURCE_DIR}/lib/LibFT4222-64.dll")
#        set(FT4222_DLL_POSTFIX "-64")
#        link_directories(${PROJECT_SOURCE_DIR}/lib)
#    endif()
#endif()

if (WIN32)
    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
        set(FT4222_DLL "${CMAKE_SOURCE_DIR}/ftdi/LibFT4222/dll/i386/LibFT4222.dll")
        set(FT4222_DLL_POSTFIX "")
        link_directories(${CMAKE_SOURCE_DIR}/ftdi/ftd2xx/i386)
        link_directories(${CMAKE_SOURCE_DIR}/ftdi/LibFT4222/dll/i386)
    else()
        set(FT4222_DLL "${CMAKE_SOURCE_DIR}/ftdi/LibFT4222/dll/amd64/LibFT4222-64.dll")
        set(FT4222_DLL_POSTFIX "-64")
        link_directories(${CMAKE_SOURCE_DIR}/ftdi/ftd2xx/amd64)
        link_directories(${CMAKE_SOURCE_DIR}/ftdi/LibFT4222/dll/amd64)
    endif()
endif()


add_executable(LaserControl
                src/FtdiI2C.cpp
                src/LaserControl.cpp
                src/debuger.cpp
              )

configure_file(include/config.h.in config.h)

if(SHOW_DEBUG_INFO)
    target_compile_definitions(LaserControl PRIVATE "SHOW_DEBUG_INFO")
endif()

              
target_include_directories(LaserControl PUBLIC 
                                "${PROJECT_SOURCE_DIR}/include"
                                "${PROJECT_BINARY_DIR}"
                          )

target_link_libraries(LaserControl PUBLIC
                        ftd2xx 
                        LibFT4222${FT4222_DLL_POSTFIX}
                     )

# To copy FT4222-xx.dll to build directory
add_custom_command(TARGET LaserControl POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${FT4222_DLL}"
                    "$<TARGET_FILE_DIR:LaserControl>/."
                  ) 

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MINGW)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O0")
endif()