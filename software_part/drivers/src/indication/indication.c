/**
 * @file    indication.c
 * @brief   Реалізація внутрішнього драйвера LED-індикації
 * @author  Olexandr Makedonskyi
 * 
 * Модуль реалізує внутрішні функції для керування LED через
 * зсувовий регістр. Забезпечує абстракцію між високорівневим
 * API та низькорівневим драйвером shift_register.
 * 
 */


//==================== INCLUDES ========================

#include "indication.h"
#include "shift_register.h"

//============ PUBLIC FUNCTION IMPLEMENTATIONS =========


void indication_initialize(void){
    // Ініціалізація апаратного інтерфейсу зсувового регістру
    shift_reg_init();
    
    // Встановлення початкового стану (всі LED вимкнуті)
    shift_reg_send(LED_ALL_OFF);
}


void indication_set_state(uint8_t state){
    shift_reg_send(state);
}