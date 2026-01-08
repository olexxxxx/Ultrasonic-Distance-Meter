#ifndef DELAYS_H_
#define DELAYS_H_
#include "stm8s.h"


void timers_for_delay_enable(void);
void _delay_us(uint16_t microseconds);
void _delay_ms(uint16_t milliseconds);
#endif