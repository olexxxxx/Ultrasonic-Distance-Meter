#ifndef DELAYS_H_
#define DELAYS_H_
#include "stm8s.h"


#define TIM4_PRESCALER_1 ((uint8_t)0x00)
#define TIM4_CR1_CEN     ((uint8_t)0x01)
#define TIM4_SR1_UIF     ((uint8_t)0x01)

void enable_timers_for_delay(void);
void _delay_us(uint16_t microseconds);
void _delay_ms(uint16_t milliseconds);
#endif