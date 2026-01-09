#ifndef ADC_H
#define ADC_H
#include "stm8s.h"

#define ADC1_CHANNEL_3   (uint8_t)0x03
#define ADC1_CR2_ALIGN   ((uint8_t)0x08)
#define ADC1_CR1_ADON    ((uint8_t)0x01)
#define ADC1_CSR_EOC     ((uint8_t)0x80)

// Ідентифікатори кнопок (залежать від напруги)
typedef enum {
    BTN_NONE = 0,
    BTN_MODE,   // Кнопка 1 (Режим/Вихід)
    BTN_UP,     // Кнопка 2 (+10)
    BTN_DOWN    // Кнопка 3 (-10)
} Button_ID;

// Режими роботи пристрою (State Machine)
typedef enum {
    STATE_MEASURE,  // Звичайний режим (далекомір)
    STATE_SETUP     // Режим налаштування порогу
} System_State;

// Глобальні змінні
extern volatile Button_ID current_button;
extern System_State current_state;
extern uint16_t threshold_val;
extern uint8_t units;
extern uint8_t button_pressed_flag;

void pd2_adc_init(void);
void adc_start_conversion(void);
unsigned long adc_read(void);
uint16_t convert_treshold(uint8_t unit, uint16_t threshold);
void show_threshold(uint16_t threshold);


#endif