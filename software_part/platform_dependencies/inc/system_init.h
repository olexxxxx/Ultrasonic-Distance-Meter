/**
 * @file    system_init.h
 * @author  Olexandr Makedonskyi
 * @brief   Функція для запуску та налаштування апаратної частини
 * @date    13.01.2026
 * @version 1.0
*/
#ifndef __SYSTEM_INIT_H
#define __SYSTEM_INIT_H



//=================== INCLUDES ============================
#include "buttons_handle.h"
#include "led_indication.h"
#include "display.h"
#include "distance_sensor.h"
#include "logger.h"
//================== FUNCTION PROTOTYPES ==================

/**
 * @brief Повна ініціалізація апаратної платформи
 * 
 * Виконує всі необхідні кроки для підготовки мікроконтролера
 * до роботи. Після виклику цієї функції система починає повноцінно
 * функціонувати.
 * 
 * Виконувані операції:
 * 1. Налаштування системного годинника (HSI 16 МГц)
 * 2. Ініціалізація таймерів для затримок (TIM4)
 * 3. Базова конфігурація периферії
 * 
 * @note Має викликатись ПЕРШОЮ при старті програми
 * 
 * @warning Повторний виклик НЕ рекомендується - може
 *          призвести до скидання налаштувань периферії
 * 
 * 
 * @see enable_system_clock()
 * @see enable_delay_timer()
 */
void initialize_hardware(void);

#endif /* __SYSTEM_INIT_H */
