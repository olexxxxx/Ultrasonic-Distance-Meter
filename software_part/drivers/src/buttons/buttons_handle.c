//==================== INCLUDES ========================

#include "buttons.h"

//============ PUBLIC FUNCTION IMPLEMENTATIONS =========


ButtonID button_read(void){
    uint16_t adc_value;
    adc_start_single_conversion();
    adc_value = adc_read();
    return button_decode(adc_value);
}



