/**
 * @file    logger.h
 * @author  Olexandr Makedonskyi
 * @brief   Інтерфейс системи логування через UART1 TX
 * @date    13.01.2026
 * @version 1.0
 * 
 * 
 * Модуль забезпечує простий інтерфейс для виведення відладочної
 * інформації по UART. Активується через logger_config.h
 * 
 * @note Якщо LOGGER_UART_ENABLE не визначено, всі функції стають no-op
 */


#ifndef __LOGGER_H
#define __LOGGER_H
//==================== INCLUDES ========================
#include "stm8s.h"

//================== FUNCTIONS PROTOTYPES ==================

/**
 * @brief Ініціалізація периферії для відправки відладочної інформації(UART) 
 *
 * Встановлює UART TX на PD5, 9600 baud rate.
 */
void initialize_logger(void); //Ініціалізує лише UART TX,  PD5, 9600 baud

/**
 * @brief Відправляє текст по UART TX.
 *
 * @param msg "Null-terminated" строка.
 */
void write_message_in_logger(const char *msg);

/**
 * @brief Відправка числа в лог
 * 
 * Конвертує ціле число в десятковий формат та відправляє по UART TX
 * з автоматичним додаванням переносу рядка.
 * 
 * @param[in] number Ціле число для логування (-2147483648 до 2147483647)
 * 
 * @note Автоматично додає '\r\n' в кінці
 * @note Якщо LOGGER_UART_ENABLE не визначено, функція нічого не робить
 * @note Блокуюча функція
 * 
 * @see write_message_in_logger()
 * @see uart1_tx_number()
 */
void write_number_in_logger(int32_t number);

#endif