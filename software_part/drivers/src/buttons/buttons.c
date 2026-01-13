/**
 * @file    buttons.c
 * @brief   Реалізація драйвера кнопок через резистивну драбину
 * @author  Olexandr Makedonskyi
 * 
 * Модуль реалізує зчитування кнопок через резистивну драбину
 * підключену до ADC. Підтримує розпізнавання трьох кнопок на
 * одному аналоговому вході.
 * 
 * Апаратна конфігурація:
 * - ADC Channel: ADC1 Channel 3 (PD2)
 * - Резистивна драбина: 4 кнопки (MODE, UP, DOWN, NONE)
 * - Роздільна здатність ADC: 10 біт (0-1023)
 */

//==================== INCLUDES ========================
#include "buttons.h"
#include "adc.h"

//==================== DEFINES =========================


/**
 * @brief Поріг ADC для кнопки MODE
 * 
 * Значення ADC вище цього порогу інтерпретується як
 * натискання кнопки MODE.
 */
#define ADC_THRESHOLD_MODE          920

/**
 * @brief Мінімальний поріг ADC для кнопки DOWN
 * 
 * Нижня межа діапазону для розпізнавання кнопки DOWN.
 */
#define ADC_THRESHOLD_DOWN_MIN      580

/**
 * @brief Максимальний поріг ADC для кнопки DOWN
 * 
 * Верхня межа діапазону для розпізнавання кнопки DOWN.
 */
#define ADC_THRESHOLD_DOWN_MAX      870

/**
 * @brief Мінімальний поріг ADC для кнопки UP
 * 
 * Нижня межа діапазону для розпізнавання кнопки UP.
 * Верхня межа неявно задана як ADC_THRESHOLD_DOWN_MIN.
 */
#define ADC_THRESHOLD_UP_MIN        100

//============ PUBLIC FUNCTION IMPLEMENTATIONS =========



void initialize_buttons(void){
    initialize_adc();
}


ButtonID button_decode(uint16_t adc_value){
    if(adc_value > ADC_THRESHOLD_MODE){
            return BTN_MODE;
        }
        else if(adc_value > ADC_THRESHOLD_DOWN_MIN &&  adc_value < ADC_THRESHOLD_DOWN_MAX){
            return BTN_DOWN;
        }
        else if(adc_value > ADC_THRESHOLD_UP_MIN && adc_value < ADC_THRESHOLD_DOWN_MIN){
            return BTN_UP;
        } 
        else {
            return BTN_NONE; 
        }
}

