/**
 * @file    sensor_internal.h
 * @author  Olexandr Makedonskyi
 * @brief   Внутрішній заголовок драйвера датчика відстані
 * @date    13.01.2026
 * @version 1.0
 * 
 * Цей файл містить внутрішні функції для роботи з датчиком
 * відстані. Забезпечує абстракцію над низькорівневим драйвером
 * конкретного датчика (HC-SR04).
 * 
 * Апаратна конфігурація:
 * - Датчик: HC-SR04 (ультразвуковий датчик відстані)
 * - TRIG: PB4 (GPIO вихід для запуску вимірювання)
 * - ECHO: PA3 (TIM2_CH3 для вимірювання тривалості імпульсу)
 * - Таймер: TIM2 з prescaler для 1 мкс
 * 
 * @note Цей файл НЕ повинен включатись напряму в інші модулі!
 *       Використовуйте distance_sensor.h замість цього.
 */

#ifndef __SENSOR_INTERNAL_H
#define __SENSOR_INTERNAL_H

//==================== INCLUDES ========================


#include "distance_sensor.h"

//================== FUNCTIONS PROTOTYPES ==================

/**
 * @brief Ініціалізація підсистеми датчика
 * 
 * Виконує ініціалізацію GPIO та таймера для роботи з HC-SR04.
 * Делегує виклики до низькорівневого HAL драйвера.
 * 
 * @note Викликається з distance_sensor_init()
 * 
 * @see distance_sensor_init()
 */
void sensor_initialize(void);

/**
 * @brief Вимірювання тривалості імпульсу ECHO
 * 
 * Виконує повний цикл вимірювання відстані:
 * 1. Генерація тригерного імпульсу
 * 2. Захоплення rising edge ECHO
 * 3. Захоплення falling edge ECHO
 * 4. Обчислення тривалості імпульсу
 * 
 * @param[in] timeout_us Максимальний час очікування в мікросекундах
 * 
 * @return Тривалість імпульсу ECHO в мікросекундах
 * @retval 0 Таймаут або помилка вимірювання
 * @retval >0 Виміряна тривалість імпульсу
 * 
 * @note Функція блокуюча
 * 
 * @see distance_sensor_measure_raw()
 */
uint16_t sensor_measure_pulse(uint32_t timeout_us);

/**
 * @brief Конвертація часу у сантиметри
 * 
 * Детальна реалізація описана в distance_sensor.h
 * 
 * @param[in] time_us Час у мікросекундах
 * @return Відстань в сантиметрах
 * 
 * @see distance_sensor_convert_to_cm()
 */
uint16_t sensor_convert_to_cm(uint16_t time_us);

/**
 * @brief Конвертація часу у дюйми
 * 
 * Детальна реалізація описана в distance_sensor.h
 * 
 * @param[in] time_us Час у мікросекундах
 * @return Відстань в дюймах
 * 
 * @see distance_sensor_convert_to_inch()
 */
uint16_t sensor_convert_to_inch(uint16_t time_us);

#endif /* __SENSOR_H */