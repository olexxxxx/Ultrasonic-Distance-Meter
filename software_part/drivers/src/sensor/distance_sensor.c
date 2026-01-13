/**
 * @file    distance_sensor.c
 * @brief   Реалізація публічного API датчика відстані
 * @author  Olexandr Makedonskyi
 * 
 * Файл містить реалізацію високорівневих функцій для роботи з
 * датчиком відстані. Забезпечує абстракцію від деталей роботи
 * конкретного датчика та таймерів.
 * 
 */

//==================== INCLUDES ========================

#include "sensor_internal.h"

//============ PUBLIC FUNCTION IMPLEMENTATIONS =========


void initialize_distance_sensor(void){
    sensor_initialize();
}


uint16_t distance_sensor_measure_raw(void){
    return sensor_measure_pulse(DISTANCE_SENSOR_TIMEOUT_US);
}


uint16_t distance_sensor_convert_to_cm(uint16_t raw_time){
    return sensor_convert_to_cm(raw_time);
}


uint16_t distance_sensor_convert_to_inch(uint16_t raw_time){
    return sensor_convert_to_inch(raw_time);
}