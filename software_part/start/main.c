
#include "stm8s.h"
#include "sys_clk.h"
#include "delays.h"
#include "tm1637.h"
#include "hc_sr04.h"
#include <stdio.h>
#include "adc.h"
#include "buttons.h"
#include "indication.h"
#include "shift_register.h"


int main(void) {
    uint16_t raw_distance;
    uint16_t distance;
    unsigned long adc_measurement;
    char buf[5];
    
    enable_system_clock();
    enable_timers_for_delay();
    tm_gpio_init();
    tm_set_brightness(5);
    tm_display_digits("8888");
    pd2_adc_init();
    hcsr04_gpio_init();
    indication_init();


    
    while (1) {
        adc_start_conversion();
        adc_measurement = adc_read();
        current_button = button_choose(adc_measurement);
        switch (current_state) {
                case STATE_MEASURE:
                    switch(current_button){
                        case BTN_MODE:
                            units = !units;
                            break;
                        case BTN_UP:
                            current_state = STATE_SETUP;
                            break;
                        case BTN_DOWN:
                            current_state = STATE_SETUP;
                            break;
                        default:
                            raw_distance = hcsr04_measure_pulse(TIMEOUT_US);
                             
                            if (units == 1) {
                            distance = hcsr04_convert_to_cm(raw_distance);
                            } else {
                            distance = hcsr04_convert_to_inch(raw_distance);
                            }

                            if (raw_distance == 0) {
                             tm_display_digits("----");
                            } else {
                                sprintf(buf, "%4d", distance);
                                tm_display_digits(buf);
                            }
                            start_indicate(units, threshold_val, raw_distance);
                            _delay_ms(65);
                    }
                    
                    break;            
                case STATE_SETUP:
                        show_threshold(threshold_val);
                        _delay_ms(100);
                    switch(current_button){
                            case BTN_MODE:
                            current_state = STATE_MEASURE;
                            break;
                        case BTN_UP:
                            threshold_val = (threshold_val < 400) ? threshold_val + 10 : 400;
                            break;
                        case BTN_DOWN:
                            if (threshold_val >= 10) threshold_val -= 10;
                            else threshold_val = 0; // Вимкнено
                            break;
                        default:
                            show_threshold(threshold_val);
                            break;
                    }
            }
    }
    return 0;
}
