/**
 * @file    sensor_internal.c
 * @brief   Реалізація внутрішнього драйвера датчика відстані
 * @author  Olexandr Makedonskyi
 * 
 * Модуль реалізує внутрішні функції для керування датчиком
 * відстані. Забезпечує абстракцію між високорівневим API
 * та низькорівневим HAL драйвером HC-SR04.
 * 
 */

//==================== INCLUDES ========================

#include "sensor_internal.h"
#include "hc_sr04.h"

//==================== DEFINES =========================

/**
 * @brief Коефіцієнт конвертації мікросекунд у сантиметри
 * 
 */
#define TIME_TO_CM_DIVIDER      58

/**
 * @brief Коефіцієнт конвертації мікросекунд у дюйми
 * 
 */
#define TIME_TO_INCH_DIVIDER    148

//============ PUBLIC FUNCTION IMPLEMENTATIONS =========


void sensor_initialize(void){
    hcsr04_gpio_init();
}

uint16_t sensor_measure_pulse(uint32_t timeout_us){
    return hcsr04_measure_pulse(timeout_us);
}

uint16_t sensor_convert_to_cm(uint16_t time_us){
    uint32_t distance;
    
    // Перевірка на відсутність об'єкта
    if (time_us == 0) {
        return 0;
    }
    
    // Конвертація: distance = time / 58
    distance = (uint32_t)time_us / TIME_TO_CM_DIVIDER;
    
    // Обмеження максимального значення для надійності
    return (distance > DISTANCE_SENSOR_MAX_CM) ? 
           DISTANCE_SENSOR_MAX_CM : (uint16_t)distance;
}


uint16_t sensor_convert_to_inch(uint16_t time_us){
    uint32_t distance;
    
    // Перевірка на відсутність об'єкта
    if (time_us == 0) {
        return 0;
    }
    
    // Конвертація: distance = time / 148
    distance = (uint32_t)time_us / TIME_TO_INCH_DIVIDER;
    
    // Обмеження максимального значення для надійності
    return (distance > DISTANCE_SENSOR_MAX_INCH) ? 
           DISTANCE_SENSOR_MAX_INCH : (uint16_t)distance;
}