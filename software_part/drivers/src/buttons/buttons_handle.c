//==================== INCLUDES ========================

#include "buttons.h"

//===================== DEFINES =======================

#define DEBOUNCE_DELAY_MS       20

//============= STATIC INTERNAL VARIABLES ==============

static ButtonID debounce_button(ButtonID current_button_value);
static ButtonID button_read(void);

//============ PUBLIC FUNCTION IMPLEMENTATIONS =========

ButtonID get_button_value(void){
    ButtonID button_value;
    button_value = button_read();
    return debounce_button(button_value);
}

//============= STATIC INTERNAL FUNCTION IMPLEMENTATIONS =======

/**
 * @brief Зчитування стану кнопки через ADC
 * 
 * Виконує одне зчитування ADC та декодування аналогового
 * значення у відповідний ідентифікатор кнопки.
 * 
 * @return Ідентифікатор зчитаної кнопки
 * @retval BTN_NONE Жодна кнопка не натиснута
 * @retval BTN_MODE Натиснута кнопка MODE
 * @retval BTN_UP   Натиснута кнопка UP
 * @retval BTN_DOWN Натиснута кнопка DOWN
 * 
 * @note Час виконання: ~100 мкс при 16 МГц
 * @note Функція НЕ виконує дебаунс
 * 
 * @see button_decode()
 */
static ButtonID button_read(void){
    ButtonID current_button;
    uint16_t adc_value;
    adc_start_single_conversion();
    adc_value = adc_read();
    current_button = button_decode(adc_value);
    return current_button;
}

/**
 * @brief Програмний дебаунс кнопки
 * 
 * Усуває помилкові спрацювання (bouncing) шляхом перевірки
 * стабільності стану кнопки. При зміні стану виконується
 * затримка та повторне зчитування для підтвердження.
 * 
 * @param[in] current_button_value Поточне значення кнопки
 * @return Підтверджене значення кнопки
 * @retval BTN_NONE Жодна кнопка не натиснута стабільно
 * @retval BTN_MODE Кнопка MODE натиснута стабільно
 * @retval BTN_UP   Кнопка UP натиснута стабільно
 * @retval BTN_DOWN Кнопка DOWN натиснута стабільно
 * 
 * @note Використовує static змінну для збереження останнього
 *       стабільного стану між викликами
 * @note Час виконання: ~0 (якщо стан не змінився) або 
 *       ~20 мс (при зміні стану)
 * 
 * @see DEBOUNCE_DELAY_MS
 * @see button_read()
 */
static ButtonID debounce_button(ButtonID current_button_value){
    static ButtonID last_stable_button = BTN_NONE;  // static!
    ButtonID confirmed_button;
    
    // Якщо зміни відсутні - повертаємо те саме значення 
    if (current_button_value == last_stable_button) {  // current_button_value
        return current_button_value;  // current_button_value
    }
    
    // В разі зміни чекаємо, після чого підтверджуємо натискання кнопки
    _delay_ms(DEBOUNCE_DELAY_MS);
    
    // Підтверджуємо натискання
    confirmed_button = button_read();
    
    // Якщо стабільно - оновлюємо 
    if (confirmed_button == current_button_value) {  // current_button_value
        last_stable_button = confirmed_button;
        return confirmed_button;
    }
    
    // Bouncing - повертаємо стабільне значення
    return last_stable_button;
}



