#ifndef LTC5100PARAMETERS_HPP
#define LTC5100PARAMETERS_HPP

#include <iostream>
#include <cstdint>
#include <cmath>
#include <FtdiI2C.hpp>

class LTC5100Parameters {
    public:

    bool Operating_mode;
    bool Soft_en;
    bool En_polarity;
    bool Apc_en;
    bool Power_down_en;
    bool Ext_temp_en;
    bool Md_polarity;
    bool Cml_en;

    bool Over_current_en;
    bool Under_pwr_en;
    bool Over_pwr_en;
    bool Force_flt;
    bool Flt_pin_override;
    bool Flt_pin_polarity;
    bool Auto_shutdn_en;
    bool Lpc_en;
    uint8_t Flt_drv_mode;
    bool Rapid_restart_en;
    bool Rep_flt_inhibit;
    
    uint8_t Ib_limit;
    uint8_t Is_rng;
    uint8_t Imd_rng;
    uint8_t Im_rng;
    uint16_t Ibmd_nom;
    uint16_t Im_nom;

    uint8_t Peaking;
    uint8_t Im_gain;
    uint8_t Ibmd_gain;

    uint16_t T_nom;
    uint16_t T_ext;
    uint8_t Ibmd_tc1;
    uint8_t Ibmd_tc2;
    uint8_t Im_tc1;
    uint8_t Im_tc2;

    uint16_t current_flt_status;


    LTC5100Parameters();                // Consturct void obj 
    LTC5100Parameters(uint8_t* memory); // Construct from EEPROM memory dump

    void update_from_EEPROM(uint8_t* memory);
    void update_from_controller(FtdiI2C* i2c);
    bool write_to_EEPROM(FtdiI2C* i2c, uint8_t eepromadr);

    void pretty_print();
    void pretty_print(bool full);
};



#endif