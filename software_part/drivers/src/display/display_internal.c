/**
 * @file    display_internal.c
 * @brief   Реалізація внутрішнього драйвера дисплея
 * @author  Olexandr Makedonskyi
 * 
 * Модуль реалізує внутрішні функції для керування дисплеєм.
 * Забезпечує абстракцію між високорівневим API
 * та низькорівневим HAL драйвером TM1637.
 * 
 */

//==================== INCLUDES ========================
#include "display_internal.h"
#include "tm1637.h"
#include <stdio.h>  // For sprintf

//==================== DEFINES =========================

/**
 * @brief Максимальна довжина рядка для обробки
 * 
 * Обмеження для уникнення переповнення буфера
 * при обчисленні довжини рядка.
 */
#define MAX_STRING_LENGTH   32


/**
 * @brief Кількість розрядів дисплея
 * 
 * 4-розрядний дисплей для відображення чисел 0-9999.
 */
#define DISPLAY_DIGITS_COUNT        4
//================ PRIVATE VARIABLES ===================

/**
 * @brief Таблиця конвертації цифр у 7-сегментний код
 * 
 * Індекс масиву = цифра (0-9)
 * Значення = біти сегментів A-G
 * 
 * Формат сегментів:
 *       A
 *      ---
 *   F |   | B
 *      -G-
 *   E |   | C
 *      ---
 *       D
 */
static const uint8_t segment_map_digits[10] = {
    0x3F,  // 0: ABCDEF
    0x06,  // 1: BC
    0x5B,  // 2: ABDEG
    0x4F,  // 3: ABCDG
    0x66,  // 4: BCFG
    0x6D,  // 5: ACDFG
    0x7D,  // 6: ACDEFG
    0x07,  // 7: ABC
    0x7F,  // 8: ABCDEFG
    0x6F   // 9: ABCDFG
};

//=============== PRIVATE FUNCTION PROTOTYPES ==========

static uint8_t char_to_segment(char c);

//============ PUBLIC FUNCTION IMPLEMENTATIONS =========


void display_driver_init(void) {
    tm1637_gpio_init();
}


void display_driver_write_text(const char *text) {
    int i;
    int index;
    int start;
    char c;
    uint8_t seg;
    int len = 0;
    uint8_t segments[DISPLAY_DIGITS_COUNT];
    
    // 1. Визначення довжини рядка (з обмеженням)
    if (text) {
        while (text[len] != '\0' && len < MAX_STRING_LENGTH) {
            ++len;
        }
    }
    
    // 2. Визначення початку для відображення
    // Якщо рядок довший за 4 символи, беремо останні 4
    if (len > DISPLAY_DIGITS_COUNT) {
        start = len - DISPLAY_DIGITS_COUNT;
    } else {
        start = 0;
    }
    
    // 3. Конвертація символів у 7-сегментний формат
    for (i = 0; i < DISPLAY_DIGITS_COUNT; ++i) {
        index = start + i;
        
        // Вибір символу або пробіл якщо вихід за межі
        if (index < len) {
            c = text[index];
        } else {
            c = ' ';
        }
        
        // Конвертація символу у сегменти
        seg = char_to_segment(c);
        segments[i] = seg;
    }
    
    // 4. Відправка сегментів до HAL драйвера
    tm1637_write_segments(segments);
}


void display_format_number(uint16_t number, char *buffer) {
    // Форматування числа з правим вирівнюванням
    // %4d додає пробіли зліва для чисел < 1000
    sprintf(buffer, "%4d", number);
}


void display_driver_set_brightness(uint8_t brightness) {
    tm1637_set_brightness(brightness);
}

//=========== PRIVATE FUNCTION IMPLEMENTATIONS =========

/**
 * @brief Конвертація символу у 7-сегментний код
 * 
 * Перетворює один текстовий символ у відповідний
 * 7-сегментний код для відображення.
 * 
 * Підтримувані символи:
 * - Цифри: '0'-'9'
 * - Літери: A, b, C, E, F, H, L, P, r, U (case insensitive)
 * - Спеціальні: ' ', '-', '_'
 * 
 * @param[in] c Символ для конвертації
 * 
 * @return 7-сегментний код
 * @retval 0x00 Для пробілу або непідтримуваного символу
 * 
 * @note Непідтримувані символи замінюються пробілом
 */
static uint8_t char_to_segment(char c) {
    // Цифри: 0-9
    if (c >= '0' && c <= '9') {
        return segment_map_digits[c - '0'];
    }
    
    // Літери та спеціальні символи
    switch (c) {
        case ' ':  return 0x00;       // Пробіл (всі вимкнені)
        case '-':  return 0x40;       // Дефіс (тільки G)
        case '_':  return 0x08;       // Підкреслення (тільки D)
        
        // Літери (case insensitive)
        case 'A': case 'a': return 0x77;  // ABCEFG
        case 'b':           return 0x7C;  // CDEFG (нижній регістр)
        case 'C': case 'c': return 0x58;  // DEG (маленьке C)
        case 'E': case 'e': return 0x79;  // ADEFG
        case 'F': case 'f': return 0x71;  // AEFG
        case 'H': case 'h': return 0x76;  // BCEFG
        case 'L':           return 0x38;  // DEF
        case 'P': case 'p': return 0x73;  // ABEFG
        case 'r':           return 0x50;  // EG (маленьке r)
        case 'U': case 'u': return 0x3E;  // BCDEF
        
        // Непідтримуваний символ - замінюємо пробілом
        default:  return 0x00;
    }
}