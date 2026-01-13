/**
 * @file    tm1637.h
 * @author  Olexandr Makedonskyi
 * @brief   HAL драйвер контролера дисплея TM1637
 * @date    13.01.2026
 * @version 1.0
 * Модуль забезпечує низькорівневий інтерфейс для роботи з
 * контролером 7-сегментного дисплея TM1637.
 * 
 * Апаратна конфігурація:
 * - Контролер: TM1637 (LED Display Driver)
 * - CLK: PC3 (GPIO вихід, Push-Pull, High Speed)
 * - DIO: PC4 (GPIO двонаправлений, Open-Drain, High Speed)
 * - Протокол: 2-wire serial interface (схожий на I²C)
 * - Дисплей: 4 розряди x 8 сегментів (7-seg + DP)
 * 
 * Протокол TM1637:
 * - START: DIO падає при HIGH CLK
 * - STOP: DIO зростає при HIGH CLK
 * - Передача байта: LSB first, 8 біт + ACK
 * - Команди: Data command (0x40), Address command (0xC0), Display control (0x80)
 * 
 * Тайминг сигналів:
 * - CLK період: мінімум 4 мкс (250 кГц)
 * - Setup time: 1 мкс
 * - Hold time: 1 мкс
 * - START/STOP затримка: 2 мкс
 * 
 * Формат 7-сегментного коду (біти):
 *  Bit:  7  6  5  4  3  2  1  0
 *       DP  G  F  E  D  C  B  A
 * 
 * Розташування сегментів:
 *       A
 *      ---
 *   F |   | B
 *      -G-
 *   E |   | C
 *      ---
 *       D    DP
 * 
 * @note Модуль призначений для використання драйверами
 *       вищого рівня (display_internal.h)
 */

#ifndef __TM1637_HAL_H
#define __TM1637_HAL_H

//==================== INCLUDES ========================
#include "stm8s.h" 
#include "delays.h"

//==================== DEFINES =========================

/**
 * @brief Порт GPIO для TM1637
 */
#define TM1637_PORT        GPIOC

/**
 * @brief Номер піна DIO (PC4)
 * 
 * Data Input/Output - двонаправлена лінія даних.
 * Режим: Open-Drain для підтримки ACK від TM1637.
 */
#define TM1637_DIO_PIN     4U

/**
 * @brief Номер піна CLK (PC3)
 * 
 * Clock - тактовий сигнал, генерується MCU.
 * Режим: Push-Pull.
 */
#define TM1637_CLK_PIN     3U

/**
 * @brief Бітова маска для піна DIO
 */
#define TM1637_DIO_MASK    ((uint8_t)(1U << TM1637_DIO_PIN))

/**
 * @brief Бітова маска для піна CLK
 */
#define TM1637_CLK_MASK    ((uint8_t)(1U << TM1637_CLK_PIN))

/**
 * @brief Кількість розрядів дисплея TM1637
 * 
 * TM1637 підтримує до 6 розрядів, але в даному
 * проєкті використовується 4-розрядний дисплей.
 */
#define TM1637_DIGITS_COUNT     4

//================ COMMAND DEFINES =====================

/**
 * @brief Команда запису даних
 * 
 * 0x40: Автоматичний інкремент адреси після кожного байта.
 * Використовується перед відправкою послідовності сегментів.
 */
#define TM1637_CMD_DATA_AUTO    0x40

/**
 * @brief Команда встановлення адреси
 * 
 * 0xC0: Початкова адреса дисплея (розряд 0).
 * Можна використовувати 0xC0-0xC3 для окремих розрядів.
 */
#define TM1637_CMD_ADDR_START   0xC0

/**
 * @brief Команда керування дисплеєм
 * 
 * 0x88: Увімкнення дисплея + яскравість 0.
 * Формат: 0x88 | (brightness & 0x07)
 */
#define TM1637_CMD_DISPLAY_ON   0x88

/**
 * @brief Маска для яскравості
 * 
 * Молодші 3 біти команди керування дисплеєм.
 */
#define TM1637_BRIGHTNESS_MASK  0x07

//=============== SEGMENT MAP DEFINES ==================

/**
 * @brief Порожній сегмент (всі вимкнені)
 */
#define TM1637_SEG_BLANK    0x00

/**
 * @brief Сегмент дефісу (тільки G)
 */
#define TM1637_SEG_DASH     0x40

//================== FUNCTION PROTOTYPES ==================

/**
 * @brief Ініціалізація GPIO для роботи з TM1637
 * 
 * Налаштовує піни CLK та DIO для роботи з контролером:
 * 
 * CLK (PC3) налаштування:
 * - DDR=1: вихід
 * - CR1=1: Push-Pull
 * - CR2=1: High Speed (10 МГц)
 * - ODR=1: початковий стан HIGH
 * 
 * DIO (PC4) налаштування:
 * - DDR=1: початково вихід
 * - CR1=0: Open-Drain (для ACK від TM1637)
 * - CR2=1: High Speed (10 МГц)
 * - ODR=1: початковий стан HIGH
 * 
 * @note Функція має викликатись один раз перед першим
 *       використанням tm1637_write_*()
 * 
 * @warning Після ініціалізації GPIO необхідно ініціалізувати
 *          дисплей викликом tm1637_set_brightness()
 * 
 * @see tm1637_set_brightness()
 * @see tm1637_write_segments()
 */
void tm1637_gpio_init(void);


/**
 * @brief Встановлення яскравості дисплея
 * 
 * Відправляє команду керування дисплеєм для зміни яскравості.
 * 
 * 
 * @param[in] brightness Рівень яскравості (0-7)
 *                       Якщо > 7, буде обмежено до 7
 * 
 * @note Зміна яскравості відбувається миттєво
 * @note Команда також увімкає дисплей (Display ON)
 * 
 * @see tm1637_write_segments()
 */
void tm1637_set_brightness(uint8_t brightness);

/**
 * @brief Запис послідовності сегментів на дисплей
 * 
 * Відправляє масив 7-сегментних кодів для відображення
 * на всіх 4 розрядах дисплея.
 * 
 * Послідовність команд:
 * 1. START
 * 2. Data command (0x40) - режим автоінкременту адреси
 * 3. STOP
 * 4. START
 * 5. Address command (0xC0) - початкова адреса (розряд 0)
 * 6. 4 байти сегментів (digit 0-3)
 * 7. STOP
 * 
 * Формат сегментного коду:
 * - Біти 0-6: Сегменти A-G
 * - Біт 7: Десяткова крапка (DP)
 * 
 * @param[in] segments Вказівник на масив з 4 байт сегментів
 *                     segments[0] = розряд 0 (лівий)
 *                     segments[1] = розряд 1
 *                     segments[2] = розряд 2
 *                     segments[3] = розряд 3 (правий)
 * 
 * @warning Масив segments має містити рівно 4 байти
 * @warning Вказівник segments не може бути NULL
 * 
 * @note Функція не змінює яскравість дисплея
 * 
 * @code
 * // Приклад: Відображення "1234"
 * uint8_t segments[4] = {0x06, 0x5B, 0x4F, 0x66};
 * tm1637_write_segments(segments);
 * @endcode
 * 
 * @see tm1637_set_brightness()
 */
void tm1637_write_segments(const uint8_t *segments);

#endif /* __TM1637_HAL_H */