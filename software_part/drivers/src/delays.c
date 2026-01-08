#include "delays.h"

void timers_for_delay_enable(void){
    TIM4->PSCR = TIM4_PRESCALER_1; 
    TIM4->ARR = ((16000000L)/1000000) - 1;
    TIM4->CR1 = TIM4_CR1_CEN; 
}

void _delay_us(uint16_t microseconds) {
    for (; microseconds > 1; --microseconds) {
        while ((TIM4->SR1 & TIM4_SR1_UIF) == 0);
        TIM4->SR1 &= ~TIM4_SR1_UIF;
    }
}

void _delay_ms(uint16_t milliseconds)
{
    while (milliseconds)
    {
        _delay_us(1000);
        milliseconds--;
    }
}
