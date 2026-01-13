/**
 * @file    system_init.c
 * @author  Olexandr Makedonskyi
 * @brief   Реалізація ініціалізації апаратної платформи
 * @date    13.01.2026
 * @version 1.0
 * 
 * Модуль виконує централізовану ініціалізацію всіх базових
 * апаратних ресурсів мікроконтролера STM8S. Забезпечує
 * детермінований порядок ініціалізації.
 * 
 */
//==================== INCLUDES ========================
#include "system_init.h"

//=============== INTERNAL FUNCTION DEFINES =============

#define TIM4_PRESCALER_1 ((uint8_t)0x00)

//=============== INTERNAL FUNCTION PROTOTYPES ==========

static void enable_system_clock(void);
static void enable_timers_for_delay(void);


//============ PUBLIC FUNCTION IMPLEMENTATIONS =========

void initialize_hardware(void){
    enable_system_clock();
    enable_timers_for_delay();
    initialize_display();
    initialize_buttons();
    initialize_distance_sensor();
    led_indication_init();
    initialize_logger();
    
}

//=========== INTERNAL FUNCTION IMPLEMENTATIONS ========


/**

  * @brief Ініціалізація тактування системи

  * @param None

  * @retval None

  * @par HSI 16Mhz

  */
static void enable_system_clock(void){
    /* Enable HSI */
    CLK->ICKR |= 0x01; // HSIEN
    /* Wait until HSI ready (HSIRDY bit) */
    while ((CLK->ICKR & 0x02) == 0) {;} // HSIRDY
    CLK->CKDIVR &= (uint8_t)~(0x18 | 0x07);

}

/**
 * @brief Внутрішня ініціалізація таймера для затримок
 * 
 * Налаштовує TIM4 для створення мікросекундних затримок.
 * Таймер працює в режимі polling (без переривань).
 * 
 * 
 * @note Функція ПРИВАТНА - викликається тільки з initialize_hardware()
 * 
 */
static void enable_timers_for_delay(void){
    TIM4->PSCR = TIM4_PRESCALER_1; 
    TIM4->ARR = ((HSI_VALUE)/1000000) - 1;
    TIM4->CR1 = TIM4_CR1_CEN; 
}
