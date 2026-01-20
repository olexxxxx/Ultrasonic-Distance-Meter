/**
  *
  * @file    delays.h
  * @author  Olexandr Makedonskyi
  * @brief   Інтерфейс модуля програмних затримок.
  * @date    13.01.2026
  * @version 1.0
  * 
  * Модуль надає функції для створення блокуючих затримок
  * мікросекундного та мілісекундного діапазону, використовуючи
  * апаратний таймер TIM4.
  *
  */

#ifndef DELAYS_H_
#define DELAYS_H_

//==================== INCLUDES ========================
#include "stm8s.h"
//================== FUNCTIONS PROTOTYPES ==================

/**
 * @brief Затримка у мікросекундах
 * Функція виконує блокуючу затримку на задану кількість мікросекунд.
 * Використовує прапор оновлення таймера TIM4.
 * @param[in] microseconds Час затримки у мкс (us).
 * 
 */
void _delay_us(uint16_t microseconds);

/**
 * @brief Затримка у мілісекундах
 * Обгортка над delay_us для формування довших інтервалів.
 * @param[in] milliseconds Час затримки у мс (ms).
 */
void _delay_ms(uint16_t milliseconds);
#endif /* DELAYS_H */