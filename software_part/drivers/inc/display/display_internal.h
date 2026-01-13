/**
 * @file    display_internal.h
 * @author  Olexandr Makedonskyi
 * @brief   Внутрішній заголовок драйвера дисплея
 * @date    13.01.2026
 * @version 1.0
 * 
 * Цей файл містить внутрішні функції для роботи з дисплеєм.
 * Забезпечує абстракцію над низькорівневим драйвером
 * конкретного контролера (TM1637).
 * 
 * 
 * @note Цей файл НЕ повинен включатись напряму в інші модулі!
 *       Використовуйте display.h замість цього.
 */

#ifndef __DISPLAY_INTERNAL_H
#define __DISPLAY_INTERNAL_H


//==================== INCLUDES ========================
#include "stm8s.h"  
//================== FUNCTION PROTOTYPES ==================

/**
 * @brief Ініціалізація підсистеми дисплея
 * 
 * Виконує ініціалізацію GPIO та контролера TM1637.
 * Делегує виклики до низькорівневого HAL драйвера.
 * 
 * @note Викликається з initialize_display()
 * 
 * @see initialize_display()
 */
void display_driver_init(void);

/**
 * @brief Відправка тексту на дисплей
 * 
 * Конвертує текстовий рядок у 7-сегментний формат
 * та передає до HAL драйвера для відображення.
 * 
 * 
 * @param[in] text Null-terminated рядок для відображення
 * 
 * @note Функція не перевіряє text на NULL
 * 
 * @see display_show_text()
 */
void display_driver_write_text(const char *text);

/**
 * @brief Конвертація числа у текстовий рядок
 * 
 * Форматує число у 4-символьний рядок з правим
 * вирівнюванням та веде пробілами зліва.
 * 
 * 
 * @param[in]  number Число для конвертації (0-65535)
 * @param[out] buffer Буфер для результату (мінімум 5 байт)
 * 
 * @note Буфер має бути достатнього розміру (5 байт)
 * 
 * @see display_show_number()
 */
void display_format_number(uint16_t number, char *buffer);

/**
 * @brief Встановлення яскравості дисплея
 * 
 * Передає команду зміни яскравості до HAL драйвера.
 * 
 * @param[in] brightness Рівень яскравості (0-7)
 * 
 * @note Викликається з display_set_brightness()
 * 
 * @see display_set_brightness()
 */
void display_driver_set_brightness(uint8_t brightness);

#endif /* __DISPLAY_INTERNAL_H */