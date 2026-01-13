/**
 * @file    hc_sr04.c
 * @author  Olexandr Makedonskyi
 * @brief   Реалізація HAL драйвера HC-SR04
 * @date    13.01.2026
 * @version 1.0
 * 
 * Модуль реалізує низькорівневі функції для керування
 * ультразвуковим датчиком HC-SR04 через GPIO та таймер TIM2.
 * 
 * Особливості реалізації:
 * - Input Capture режим таймера для точного вимірювання
 * - Програмна затримка для тригерного імпульсу
 * - Polling режим очікування фронтів
 * - Відновлення стану після таймауту
 * 
 * Тайминг операцій:
 * - Тригерний імпульс: 10 мкс
 * - Очікування rising edge: до timeout_us
 * - Очікування falling edge: до timeout_us
 * - Загальний час: 20 мкс + час відбиття (до 30 мс)
 */

//==================== INCLUDES ========================

#include "hc_sr04.h"

//============= STATIC INTERNAL FUNCTIONS PROTOTYPES ==============

static void hcsr04_send_trigger(void);
static void tim2_ch3_enable(void);

//============ PUBLIC FUNCTIONS =========================



void hcsr04_gpio_init(void) {
    // Налаштування ECHO (PA3) як вхід для TIM2_CH3
    // DDR=0: вхід
    // CR1=0: floating (без підтяжки)
    HCSR04_ECHO_PORT->DDR &= ~HCSR04_ECHO_PIN;
    HCSR04_ECHO_PORT->CR1 &= ~HCSR04_ECHO_PIN;
    
    // Налаштування TRIG (PB4) як вихід Push-Pull High Speed
    // DDR=1: вихід
    // CR1=1: Push-Pull
    // CR2=1: High Speed (10 МГц)
    HCSR04_TRIG_PORT->DDR |= HCSR04_TRIG_PIN;
    HCSR04_TRIG_PORT->CR1 |= HCSR04_TRIG_PIN;
    HCSR04_TRIG_PORT->CR2 |= HCSR04_TRIG_PIN;
    
    // Встановлення початкового стану TRIG = LOW
    HCSR04_TRIG_PORT->ODR &= ~HCSR04_TRIG_PIN;
    
    // Ініціалізація таймера для Input Capture
    tim2_ch3_enable();
}


uint16_t hcsr04_measure_pulse(uint32_t timeout_us) {
    uint16_t rise_time;
    uint16_t fall_time;
    uint16_t timeout_counter = 0;
    
    // 1. Скидання прапорця CC3IF перед початком вимірювання
    TIM2->SR1 &= ~TIM2_SR1_CC3IF;
    
    // 2. Налаштування на захоплення rising edge
    // CCER2 = 0x01: CC3E=1 (enable), CC3P=0 (rising edge)
    TIM2->CCER2 = 0x01;
    
    // 3. Генерація тригерного імпульсу
    hcsr04_send_trigger();
    
    // 4. Очікування rising edge ECHO (початок імпульсу)
    while ((TIM2->SR1 & TIM2_SR1_CC3IF) == 0) {
        if (++timeout_counter >= timeout_us) {
            return 0;  // Таймаут - об'єкт не виявлено
        }
    }
    
    // 5. Зчитування часу rising edge (молодший байт першим!)
    rise_time = (TIM2->CCR3H << 8) | TIM2->CCR3L;
    
    // 6. Перемикання на захоплення falling edge
    // Скидання прапорця перед зміною режиму
    TIM2->SR1 &= ~TIM2_SR1_CC3IF;
    // CCER2 = 0x03: CC3E=1 (enable), CC3P=1 (falling edge)
    TIM2->CCER2 = 0x03;
    
    // 7. Очікування falling edge ECHO (кінець імпульсу)
    timeout_counter = 0;
    while ((TIM2->SR1 & TIM2_SR1_CC3IF) == 0) {
        if (++timeout_counter >= timeout_us) {
            // Таймаут - відновлення початкового стану
            TIM2->CCER2 = 0x01;
            return 0;
        }
    }
    
    // 8. Зчитування часу falling edge (молодший байт першим!)
    fall_time = (TIM2->CCR3H << 8) | TIM2->CCR3L;
    
    // 9. Відновлення початкового стану (rising edge для наступного вимірювання)
    TIM2->CCER2 = 0x01;
    
    // 10. Обчислення тривалості імпульсу
    // Автоматично враховує переповнення таймера (16-bit arithmetic)
    return (uint16_t)(fall_time - rise_time);
}

//============= STATIC INTERNAL FUNCTION IMPLEMENTATIONS =======

/**
 * @brief Генерація тригерного імпульсу для запуску вимірювання
 * 
 * Формує імпульс HIGH тривалістю 10 мкс на піні TRIG.
 * Після імпульсу датчик HC-SR04 починає генерувати
 * ультразвукові імпульси та встановлює ECHO в HIGH.
 * 
 * Послідовність:
 * 1. TRIG = HIGH
 * 2. Затримка 10 мкс
 * 3. TRIG = LOW
 * 
 * @note Після виклику необхідно очікувати сигнал ECHO
 * 
 * @warning Не викликати повторно до завершення попереднього
 *          вимірювання (мінімум 60 мс інтервал)
 * 
 * @see hcsr04_measure_pulse()
 */
static void hcsr04_send_trigger(void) {
    // 1. Встановлення TRIG = HIGH
    HCSR04_TRIG_PORT->ODR |= HCSR04_TRIG_PIN;
    
    // 2. Затримка 10 мкс (мінімум за специфікацією HC-SR04)
    _delay_us(HCSR04_TRIGGER_PULSE_US);
    
    // 3. Скидання TRIG = LOW
    HCSR04_TRIG_PORT->ODR &= ~HCSR04_TRIG_PIN;
}

/**
 * @brief Ініціалізація TIM2 Channel 3 для Input Capture
 * 
 * Налаштовує TIM2_CH3 для вимірювання тривалості імпульсу ECHO:
 * - Prescaler: /16 (16 МГц / 16 = 1 МГц)
 * - Роздільна здатність: 1 мкс
 * - Режим: Input Capture (захоплення фронтів)
 * - Auto-reload: максимум (65535 мкс)
 * 
 * @note Викликається автоматично з hcsr04_gpio_init()
 * 
 * @see hcsr04_gpio_init()
 */
static void tim2_ch3_enable(void) {
    // 1. Зупинка таймера для безпечної конфігурації
    TIM2->CR1 = 0;
    
    // 2. Налаштування prescaler: PSC = 16
    // Частота таймера: 16 МГц / 16 = 1 МГц → 1 тік = 1 мкс
    // PSCR = 0x04 означаєділення на 2^4 = 16
    TIM2->PSCR = 0x04;
    
    // 3. Налаштування auto-reload на максимум
    // ARR = 0xFFFF → період 65535 мкс (65.5 мс)
    TIM2->ARRH = 0xFF;
    TIM2->ARRL = 0xFF;
    
    // 4. Скидання прапорців статусу (уникнення phantom interrupt)
    TIM2->SR1 = 0;
    TIM2->SR2 = 0;
    
    // 5. Налаштування CH3 в режим Input Capture
    // CCMR3 = 0x01: CC3S=01 (TI3 mapped on TI3FP3)
    TIM2->CCMR3 = 0x01;
    
    // 6. Запуск таймера
    // CEN = 1 (Counter Enable)
    TIM2->CR1 |= 0x01;
}