/**
 * @file    delays.c
 * @author  Olexandr Makedonskyi
 * @brief   Реалізація функцій програмних затримок
 * @date    13.01.2026
 * @version 1.0
 * 
 * * Файл містить низькорівневу реалізацію затримок на базі
 * периферійного таймера TIM4 мікроконтролера STM8.
 */

//==================== INCLUDES ========================
#include "delays.h"


//==================== DEFINES =================

#define TIM4_PRESCALER_1    ((uint8_t)0x00)
#define TIM4_CR1_CEN        ((uint8_t)0x01)
#define TIM4_SR1_UIF        ((uint8_t)0x01)

//============ PUBLIC FUNCTIONS IMPLEMENTATION =========

void _delay_us(uint16_t microseconds) {
    /* Цикл зменшення лічильника. 
       Логіка базується на очікуванні прапора Update Interrupt Flag (UIF) */
    for (; microseconds > 1; --microseconds) {
        /* Очікування встановлення прапора переповнення */
        while ((TIM4->SR1 & TIM4_SR1_UIF) == 0);
        /* Очищення прапора перед очікуванням*/
        TIM4->SR1 &= ~TIM4_SR1_UIF;
    }
}

void _delay_ms(uint16_t milliseconds)
{
    while (milliseconds)
    {
        _delay_us(1000);
        milliseconds--;
    }
}
