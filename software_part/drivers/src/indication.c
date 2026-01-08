#include "indication.h"


void indication_init(void){
    shift_reg_init(); 
    shift_reg_send(0x00); 
}

void start_indicate(uint8_t units, uint16_t threshold, uint16_t raw_distance){
    uint16_t distance_cm;
    uint16_t distance_inch;
    uint16_t thrshld_converted;
    switch(units){
        case 1:
            distance_cm = hcsr04_convert_to_cm(raw_distance);
            thrshld_converted = convert_treshold(1, threshold);
            if(thrshld_converted > distance_cm){
                shift_reg_send(0xFF);
            }else{
                shift_reg_send(0x00);
            }
            break;
        case 0:
            distance_inch = hcsr04_convert_to_inch(raw_distance);
            thrshld_converted = convert_treshold(0, threshold);
            if(thrshld_converted > distance_inch){
                shift_reg_send(0xFF);
            }else{
                shift_reg_send(0x00);
            }
            break;
        }

    }
