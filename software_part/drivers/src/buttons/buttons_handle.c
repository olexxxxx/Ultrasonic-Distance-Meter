//==================== INCLUDES ========================

#include "buttons.h"
#include "delays.h"

//=============== STATIC INTERNAL FUNCTION PROTOTYPES ==========

static void debounce(void);

//============ PUBLIC FUNCTION IMPLEMENTATIONS =========


ButtonID button_read(void){
    uint16_t adc_value;
    adc_start_single_conversion();
    adc_value = adc_read();
    debounce();
    return button_decode(adc_value);
}

//============= STATIC INTERNAL FUNCTION IMPLEMENTATIONS =======

/**
 * @brief  Попереджує хибному спрацюванню натискання кнопки
 * @param  None
 * @retval None
 */

static void debounce(void){
    _delay_ms(250);
}