/**
 * @file    uart1_tx.h
 * @author  Olexandr Makedonskyi
 * @brief   UART1 TX-only драйвер для STM8S103
 * @date    13.01.2026
 * @version 1.0
 * 
 * Мінімалістичний драйвер для передачі даних по UART1.
 * Підтримує тільки TX (PD5), RX не використовується.
 * 
 * Характеристики:
 * - TX пін: PD5
 * - Baud rate: 9600 (за замовчуванням)
 * - Data bits: 8
 * - Parity: None
 * - Stop bits: 1
 * - Flow control: None
 * 
 */

#ifndef __UART1_TX_H
#define __UART1_TX_H

//==================== INCLUDES ========================
#include "stm8s.h"
//==================== CONSTANTS =======================

/**
 * @brief Стандартні швидкості передачі (baud rates)
 */
#define UART_BAUD_9600      9600UL
#define UART_BAUD_19200     19200UL
#define UART_BAUD_38400     38400UL
#define UART_BAUD_57600     57600UL
#define UART_BAUD_115200    115200UL

/**
 * @brief Константа для вибору baud rate, що використовується в даний момент
 */
#define UART_CURRENT_SPEED UART_BAUD_9600 
/**
 * @brief Частота системного тактування за замовчуванням (HSI)
 */
#define UART_SYSTEM_CLOCK   HSI_VALUE

//================== FUNCTION PROTOTYPES ===============

/**
 * @brief Ініціалізація UART1 для передачі
 * 
 * Налаштовує UART1 TX (PD5) з заданими параметрами:
 * - 8 data bits
 * - No parity
 * - 1 stop bit
 * - Передатчик увімкнено, приймач вимкнено
 * 
 * @param[in] baud_rate Швидкість передачі (біт/с)
 *                      Рекомендовано: UART_BAUD_9600
 * @param[in] f_master  Частота системного тактування (Гц)
 *                      Зазвичай: UART_SYSTEM_CLOCK (16 МГц)
 * 
 * @note GPIO (PD5) налаштовується автоматично
 * 
 * @warning Викликати ПІСЛЯ enable_system_clock()
 * 
 * @see uart1_tx_byte()
 * @see uart1_tx_string()
 */
void uart1_tx_init(uint32_t baud_rate, uint32_t f_master);

/**
 * @brief Відправка масиву байтів по UART
 * 
 * @param[in] data Вказівник на масив даних
 * @param[in] length Кількість байтів для передачі
 * 
 * @warning data не може бути NULL
 * @warning length має бути > 0
 * 
 * @note Блокуюча функція
 * 
 * @see uart1_tx_byte()
 * @see uart1_tx_string()
 */
void uart1_tx_buffer(const uint8_t* data, uint16_t length);

/**
 * @brief Відправка null-terminated рядка по UART
 * 
 * @param[in] str Вказівник на null-terminated рядок
 * 
 * @warning str не може бути NULL
 * @warning Рядок має закінчуватись '\0'
 * 
 * @note Не додає '\r\n' автоматично
 * @note Блокуюча функція
 * 
 * @see uart1_tx_byte()
 * @see uart1_tx_buffer()
 */
void uart1_tx_string(const char* str);


/**
 * @brief Відправка цілого числа в десятковому форматі
 * 
 * Конвертує ціле число зі знаком у ASCII та відправляє по UART.
 * Підтримує як додатні, так і від'ємні числа.
 * 
 * @param[in] number Ціле число для передачі (-2147483648 до 2147483647)
 * 
 * @note Не додає '\r\n' автоматично
 * @note Блокуюча функція
 * 
 * @see uart1_tx_string()
 */
void uart1_tx_number(int32_t number);

#endif /* UART1_TX_H */
