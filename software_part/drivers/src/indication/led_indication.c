//==================== INCLUDES ========================
#include "led_indication.h"
#include "indication.h"

//==================== DEFINES =========================

/**
 * @brief Кількість LED у індикаторі
 * 
 * Зсувовий регістр 74HC595 має 8 виходів (Q0-Q7).
 */
#define LED_COUNT   8

//==================== PRIVATE FUNCTIONS PROTOTYPES ==================

static uint8_t calculate_led_pattern(uint16_t distance, uint16_t threshold);

//============ PUBLIC FUNCTION IMPLEMENTATIONS =========


void led_indication_init(void){
    indication_initialize();
}


void led_indication_update(uint16_t distance, uint16_t threshold){
    uint8_t led_pattern;
    
    // Перевірка на вимкнену індикацію або відсутність об'єкта
    if (threshold == 0 || distance == 0) {
        indication_set_state(LED_ALL_OFF);
        return;
    }
    
    // Обчислення паттерну LED на основі відстані
    led_pattern = calculate_led_pattern(distance, threshold);
    indication_set_state(led_pattern);
}

//============ STATIC FUNCTIONS =========================

/**
 * @brief Обчислення паттерну LED на основі відстані до порогу
 * 
 * Функція реалізує прогресивну індикацію: чим ближче об'єкт
 * до порогового значення, тим більше LED увімкнуто.
 * 
 * Алгоритм:
 * 1. Якщо distance >= threshold → 0 LED (об'єкт далеко)
 * 2. Обчислюємо кількість LED: (threshold - distance) * 8 / threshold
 * 3. Генеруємо паттерн: (1 << led_count) - 1
 * 
 * 
 * @param[in] distance  Поточна відстань до об'єкта
 * @param[in] threshold Порогове значення відстані
 * 
 * @return Байт паттерну LED (0x00-0xFF)
 * @retval 0x00 Якщо distance >= threshold (всі LED вимкнуті)
 * @retval 0xFF Якщо distance близька до 0 (всі LED увімкнуті)
 * 
 * @note Функція внутрішня (static) - не експортується з модуля
 */
static uint8_t calculate_led_pattern(uint16_t distance, uint16_t threshold){
    uint8_t led_count;
    uint8_t pattern;
    
    // Якщо об'єкт за межами порогу - всі LED вимкнуті
    if (distance >= threshold) {
        return LED_ALL_OFF;
    }
    
    // Обчислення кількості LED: (threshold - distance) * 8 / threshold
    // Використовуємо uint32_t для уникнення переповнення при множенні
    led_count = (uint8_t)(((uint32_t)(threshold - distance) * LED_COUNT) / threshold);
    
    // Обмеження діапазону (на випадок помилок округлення)
    if (led_count > LED_COUNT) {
        led_count = LED_COUNT;
    }
    
    // Генерація паттерну: (1 << led_count) - 1
    if (led_count == 0) {
        pattern = 0x00;
    } else if (led_count >= LED_COUNT) {
        pattern = LED_ALL_ON;
    } else {
        pattern = (1 << led_count) - 1;
    }
    
    return pattern;
}