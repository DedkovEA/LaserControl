#include <iostream>
#include <cstdint>
#include <cmath>
#include <vector>
#include <FtdiI2C.hpp>

#include "LTC5100Parameters.hpp"

LTC5100Parameters::LTC5100Parameters() : current_flt_status(0x0) {};

LTC5100Parameters::LTC5100Parameters(uint8_t* memory) {
    this->update_from_EEPROM(memory);
};

void LTC5100Parameters::update_from_EEPROM(uint8_t* memory) {
    Operating_mode = ((memory[0] & 0x01) >> 0);
    Soft_en = ((memory[0] & 0x02) >> 1);
    En_polarity = ((memory[0] & 0x04) >> 2);
    Apc_en = ((memory[0] & 0x08) >> 3);
    Power_down_en = ((memory[0] & 0x10) >> 4);
    Ext_temp_en = ((memory[0] & 0x20) >> 5);
    Md_polarity = ((memory[0] & 0x40) >> 6);
    Cml_en = ((memory[0] & 0x80) >> 7);
    Over_current_en = ((memory[2] & 0x01) >> 0);
    Under_pwr_en = ((memory[2] & 0x02) >> 1);
    Over_pwr_en = ((memory[2] & 0x04) >> 2);
    Force_flt = ((memory[2] & 0x08) >> 3);
    Flt_pin_override = ((memory[2] & 0x10) >> 4);
    Flt_pin_polarity = ((memory[2] & 0x20) >> 5);
    Auto_shutdn_en = ((memory[2] & 0x40) >> 6);
    Lpc_en = ((memory[2] & 0x80) >> 7);

    Flt_drv_mode = ((memory[3] & 0x03) >> 0);
    Rapid_restart_en = ((memory[2] & 0x04) >> 2);
    Rep_flt_inhibit = ((memory[2] & 0x08) >> 3);

    Is_rng = ((memory[5] & 0x0C) >> 2);
    Imd_rng = ((memory[13] & 0x0C) >> 2);
    Im_rng = ((memory[9] & 0x0C) >> 2);

    Ibmd_nom = (uint16_t)memory[4] | (((uint16_t)memory[5]&0x3) << 8);
    Im_nom = (uint16_t)memory[8] | (((uint16_t)memory[9]&0x3) << 8);
    T_nom = (uint16_t)memory[12] | (((uint16_t)memory[13]&0x3) << 8);

    Ibmd_tc1 = memory[6];
    Ibmd_tc2 = memory[7];
    Im_tc1 = memory[10];
    Im_tc2 = memory[11];

    Ib_limit = (memory[1] & 0x7F);
    Peaking = (memory[15]&0x1F);
    Im_gain = (memory[14]&0x7);
    Ibmd_gain = ((memory[14]&0xF8) >> 3);
};


void LTC5100Parameters::update_from_controller(FtdiI2C* i2c) {
    // System Operating Configurations
    uint16_t SYS_CONFIG    = i2c->ReadLTCRegister16b(0x10);
    uint16_t LOOP_GAIN     = i2c->ReadLTCRegister16b(0x1E);
    uint16_t PEAKING       = i2c->ReadLTCRegister16b(0x1F);
    // Laser Setup Coefficients
    uint16_t IB            = i2c->ReadLTCRegister16b(0x15);
    uint16_t IB_TC1        = i2c->ReadLTCRegister16b(0x16);
    uint16_t IB_TC2        = i2c->ReadLTCRegister16b(0x17);
    uint16_t IM            = i2c->ReadLTCRegister16b(0x19);
    uint16_t IM_TC1        = i2c->ReadLTCRegister16b(0x1A);
    uint16_t IM_TC2        = i2c->ReadLTCRegister16b(0x1B);
    // Temperature   
    uint16_t T_EXT         = i2c->ReadLTCRegister16b(0x0D);
    uint16_t T_NOM         = i2c->ReadLTCRegister16b(0x1D);
    // Fault Monitoring and Eye Safety
    uint16_t FLT_CONFIG    = i2c->ReadLTCRegister16b(0x13);
    uint16_t FLT_STATUS    = i2c->ReadLTCRegister16b(0x12);
    uint16_t IB_LIMIT      = i2c->ReadLTCRegister16b(0x11);


    Operating_mode = ((SYS_CONFIG & 0x01) >> 0);
    Soft_en        = ((SYS_CONFIG & 0x02) >> 1);
    En_polarity    = ((SYS_CONFIG & 0x04) >> 2);
    Apc_en         = ((SYS_CONFIG & 0x08) >> 3);
    Power_down_en  = ((SYS_CONFIG & 0x10) >> 4);
    Ext_temp_en    = ((SYS_CONFIG & 0x20) >> 5);
    Md_polarity    = ((SYS_CONFIG & 0x40) >> 6);
    Cml_en         = ((SYS_CONFIG & 0x80) >> 7);

    Im_gain        = ((LOOP_GAIN & 0x07) >> 0);
    Ibmd_gain      = ((LOOP_GAIN & 0xF8) >> 3);
    
    Peaking        = ((PEAKING & 0x1F) >> 0);
    
    Ibmd_nom       = ((IB & 0x03FF) >> 0);
    Is_rng         = ((IB & 0x0C00) >> 10);
    
    Ibmd_tc1       = ((IB_TC1 & 0xFF) >> 0);
    Ibmd_tc2       = ((IB_TC2 & 0xFF) >> 0);

    Im_nom         = ((IM & 0x03FF) >> 0);
    Im_rng         = ((IM & 0x0C00) >> 10);

    Im_tc1         = ((IM_TC1 & 0xFF) >> 0);
    Im_tc2         = ((IM_TC2 & 0xFF) >> 0);

    T_ext          = ((T_EXT & 0x03FF) >> 0);

    T_nom          = ((T_NOM & 0x03FF) >> 0);
    Imd_rng        = ((T_NOM & 0x0C00) >> 10);

    Over_current_en  = ((FLT_CONFIG & 0x0001) >> 0);
    Under_pwr_en     = ((FLT_CONFIG & 0x0002) >> 1);
    Over_pwr_en      = ((FLT_CONFIG & 0x0004) >> 2);
    Force_flt        = ((FLT_CONFIG & 0x0008) >> 3);
    Flt_pin_override = ((FLT_CONFIG & 0x0010) >> 4);
    Flt_pin_polarity = ((FLT_CONFIG & 0x0020) >> 5);
    Auto_shutdn_en   = ((FLT_CONFIG & 0x0040) >> 6);
    Lpc_en           = ((FLT_CONFIG & 0x0080) >> 7);
    Flt_drv_mode     = ((FLT_CONFIG & 0x0300) >> 8);
    Rapid_restart_en = ((FLT_CONFIG & 0x0400) >> 10);
    Rep_flt_inhibit  = ((FLT_CONFIG & 0x0800) >> 11);

    current_flt_status = FLT_STATUS;

    Ib_limit       = ((IB_LIMIT & 0x7F) >> 0);
};


bool LTC5100Parameters::write_to_EEPROM(FtdiI2C* i2c, uint8_t eepromadr) {
    std::vector<uint8_t> data2write;
    data2write.push_back( (Operating_mode << 0) | (Soft_en << 1) | (En_polarity << 2) | (Apc_en << 3) |
                          (Power_down_en << 4) | (Ext_temp_en << 5) | (Md_polarity << 6) | (Cml_en << 7) );
    data2write.push_back( Ib_limit );
    data2write.push_back( (Over_current_en << 0) | (Under_pwr_en << 1) | (Over_pwr_en << 2) | (Force_flt << 3) |
                          (Flt_pin_override << 4) | (Flt_pin_polarity << 5) | (Auto_shutdn_en << 6) | (Lpc_en << 7) );
    data2write.push_back( (Flt_drv_mode << 0) | (Rapid_restart_en << 2) | (Rep_flt_inhibit << 3) );
    data2write.push_back( Ibmd_nom & 0xFF );
    data2write.push_back( ((Ibmd_nom & 0x300) >> 8) | Is_rng );
    data2write.push_back( Ibmd_tc1 );
    data2write.push_back( Ibmd_tc2 );
    data2write.push_back( Im_nom & 0xFF );
    data2write.push_back( ((Im_nom & 0x300) >> 8) | Im_rng );
    data2write.push_back( Im_tc1 );
    data2write.push_back( Im_tc2 );
    data2write.push_back( T_nom & 0xFF );
    data2write.push_back( ((T_nom & 0x300) >> 8) | Imd_rng );
    data2write.push_back( (Im_gain << 0) | (Ibmd_gain << 3) );
    data2write.push_back( Peaking );

    bool ret;
    for (uint8_t adr; adr < 16; adr++) {
        ret = i2c->Write24LC00Register8b(eepromadr, adr, data2write[adr]);
        if (!ret) {
            return false;
        }
    }
    return true;
};


void LTC5100Parameters::pretty_print() { this->pretty_print(false); };

void LTC5100Parameters::pretty_print(bool full) {

    std::cout << "=== Common driver parameters ===\n";
    std::cout << "Nominal temperature           " << (float)T_nom * 0.5 << " K\n";
    std::cout << "External temperature          " << (float)T_ext * 0.5 << " K\n";
    std::cout << "Bias current limit            " << (float)Ib_limit / 128 * (Is_rng + 1) * 9. << " mA\n";
    std::cout << "Peaking                       " << (int)Peaking << "\n";
    std::cout << "Auto power control mode       " << Apc_en << "\n";
    std::cout << "\n";

    std::cout << "Operating mode                " << Operating_mode << "\n";
    std::cout << "Soft enable                   " << Soft_en << "\n";
    std::cout << "Enable pin polarity           " << En_polarity << "\n";
    std::cout << "Power_down_en                 " << Power_down_en << "\n";
    std::cout << "External temperature in       " << Ext_temp_en << "\n";
    std::cout << "Monitor diode polarity        " << Md_polarity << "\n";
    std::cout << "Current mode logic En         " << Cml_en << "\n";

    std::cout << "\n=== Fault detection parameters ===\n";
    std::cout << "Over current enable           " << Over_current_en << "\n";
    std::cout << "Under power enable            " << Under_pwr_en << "\n";
    std::cout << "Over power enable             " << Over_pwr_en << "\n";
    std::cout << "Force fault                   " << Force_flt << "\n";
    std::cout << "Fault pin override            " << Flt_pin_override << "\n";
    std::cout << "Fault pin polarity            " << Flt_pin_polarity << "\n";
    std::cout << "Auto shutdown enable          " << Auto_shutdn_en << "\n";
    std::cout << "Laser power controller enable " << Lpc_en << "\n";

    std::cout << "Fault drive mode              " << (int)Flt_drv_mode << "\n";
    std::cout << "Rapid restart enable          " << Rapid_restart_en << "\n";
    std::cout << "Repeated fault inhibit        " << Rep_flt_inhibit << "\n";

if (Apc_en) {
    std::cout << "\n=== Monitor diode current parameters ===\n";
    std::cout << "Nominal monitor diode current " << 4.25 * pow(4., Imd_rng) * 
                  exp(log(8) * (float)Ibmd_nom / 1024) << " uA\n";
    std::cout << "Thermal coefficients          " << "Imd_tc1 = " << Ibmd_tc1 << ", Imd_tc2 = " << Ibmd_tc2 << "\n";
} else {
    std::cout << "\n=== Bias current parameters ===\n";
    std::cout << "Nominal bias current          " << (float)Ibmd_nom / 1024 * (Is_rng + 1) * 9. << " mA\n";
    std::cout << "Thermal coefficients          " << "Ib_tc1 = " << (int)Ibmd_tc1 << ", Ib_tc2 = " << (int)Ibmd_tc2 << "\n";
}

    std::cout << "\n=== Modulation current parameters ===\n";
    std::cout << "Nominal modulation current    " << (float)Im_nom / 1024 * (Im_rng + 1) * 9. << " mA\n";
    std::cout << "                               Note that only R_T/(R_T + R_LD) * Im_nom can reach laser diode\n";
    std::cout << "Thermal coefficients          " << "Im_tc1 = " << (int)Im_tc1 << ", Im_tc2 = " << (int)Im_tc2 << "\n";
    
    std::cout << "\n=== Servo control parameters ===\n";
    std::cout << ((Apc_en) ? "Bias current error gain       " : "APC error gain                ") << (int)Ibmd_gain << "/32\n";
    std::cout << "Modulation current error gain " << (int)Im_gain << "/8\n"; 

if (full) {
    std::cout << "\n=== Fault Status ===\n";
    std::cout << "Transmitter ready             " << (((current_flt_status & 0x0400) >> 10) ? "YES" : "NO") << "\n";
    std::cout << "Transmitter enabled           " << (((current_flt_status & 0x0200) >> 9) ? "YES" : "NO") << "\n";
    std::cout << "EN Pin is                     " << (((current_flt_status & 0x0100) >> 8) ? "HIGH" : "LOW") << "\n";
    if (Rep_flt_inhibit) {
        std::cout << "Faulted twice                 " << (((current_flt_status & 0x0080) >> 7) ? "YES" : "NO") << "\n";
        std::cout << "Faulted once                  " << (((current_flt_status & 0x0040) >> 6) ? "YES" : "NO") << "\n"; 
    } else {
        std::cout << "Faulted                       " << (((current_flt_status & 0x0020) >> 5) ? "YES" : "NO") << "\n";
    }

    std::cout << "Under voltage                 " << (((current_flt_status & 0x0010) >> 4) ? "YES" : "NO") << "\n";
    std::cout << "Memory load error             " << (((current_flt_status & 0x0008) >> 3) ? "YES" : "NO") << "\n";
    std::cout << "Over power                    " << (((current_flt_status & 0x0004) >> 2) ? "YES" : "NO") << "\n";
    std::cout << "Under power                   " << (((current_flt_status & 0x0002) >> 1) ? "YES" : "NO") << "\n";
    std::cout << "Over current                  " << (((current_flt_status & 0x0001) >> 0) ? "YES" : "NO") << "\n";
}

};