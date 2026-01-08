#include "buttons.h"


Button_ID button_choose(unsigned long adc_value){
    if(adc_value > 920){
            button_pressed_flag = 1;
            return BTN_MODE;
        }
        else if(adc_value > 580 &&  adc_value < 870){
            button_pressed_flag = 1;
            return BTN_DOWN;
        }
        else if(adc_value > 100 && adc_value < 580){
            button_pressed_flag = 1;
            return BTN_UP;
        } 
        else {
            return BTN_NONE; 
        }
}