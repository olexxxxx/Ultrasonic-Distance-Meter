
//==================== INCLUDES ========================
#include "display.h"
#include "display_internal.h"
#include <stdio.h>  // For sprintf

//============= STATIC INTERNAL FUNCTIONS PROTOTYPES ==============

static void display_set_brightness(uint8_t brightness);

//============ PUBLIC FUNCTION IMPLEMENTATIONS =========


void initialize_display(void) {
    display_driver_init();
    display_set_brightness(DISPLAY_BRIGHTNESS_DEFAULT);
}


void display_show_text(const char *text) {
    display_driver_write_text(text);
}


void display_show_number(uint16_t number) {
    char buffer[5];  // 4 digits + null terminator
    
    display_format_number(number, buffer);
    display_driver_write_text(buffer);
}


void display_show_threshold(uint16_t threshold) {
    // Еквівалентна функція для семантичної ясності
    display_show_number(threshold);
}


void display_clear(void) {
    display_driver_write_text("    ");
}

//============= STATIC INTERNAL FUNCTION IMPLEMENTATIONS =======

/**
 * @brief Встановлення яскравості дисплея
 * 
 * Регулює яскравість підсвічування дисплея.
 * Дозволяє адаптувати видимість до умов освітлення.
 * 
 * Рівні яскравості - від 0 (Мінімальна) до 7 (Максимальна) 
 * 
 * @param[in] brightness Рівень яскравості (0-7)
 *                       Якщо > 7, буде обмежено до 7
 * @retval None
 * @note   Зміна яскравості відбувається миттєво
 * @note   Яскравість не зберігається при вимкненні живлення
 */

static void display_set_brightness(uint8_t brightness) {
    display_driver_set_brightness(brightness);
}

