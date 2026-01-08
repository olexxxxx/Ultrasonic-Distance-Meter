#include "hc_sr04.h"



void hcsr04_gpio_init(void) {
    // Echo: PA3 input (TIM2_CH3)
    GPIOA->DDR &= ~(1 << 3);
    GPIOA->CR1 &= ~(1 << 3);
    // Trig: PB4 output push-pull fast
    GPIOB->DDR |= (1 << 4);
    GPIOB->CR1 |= (1 << 4);
    GPIOB->CR2 |= (1 << 4);
    GPIOB->ODR &= ~(1 << 4);
    
    tim2_ch3_enable();
}

void tim2_ch3_enable(void) {
    TIM2->CR1 = 0; //обнуляємо перед конфігом
    TIM2->PSCR = 0x04;  // 16 МГц / 16 = 1 МГц → 1 tick = 1 µs
    TIM2->ARRH = 0xFF;
    TIM2->ARRL = 0xFF;
    TIM2->SR1 = 0; //явно записуємо в 0 для уникнення невизначених станів
    TIM2->SR2 = 0; //явно записуємо в 0 для уникнення невизначених станів
    TIM2->CCMR3 = 0x01;  // (режим входу)
    TIM2->CR1 |= 0x01;   // CEN, інакше кажчи ініціалізуємо таймер
}

void hcsr04_send_trigger(void) {
    GPIOB->ODR |= (1 << 4);
    _delay_us(10);
    GPIOB->ODR &= ~(1 << 4);
}

uint16_t hcsr04_measure_pulse(uint32_t timeout_us) {
    uint16_t rise, fall, t = 0;
    
    TIM2->SR1 &= ~(1 << 3);
    TIM2->CCER2 = 0x01;  // rising edge
    
    hcsr04_send_trigger();
    
    // Wait rising edge
    while ((TIM2->SR1 & (1 << 3)) == 0) {
        if (++t >= timeout_us) return 0;
    }
    rise = (TIM2->CCR3H << 8) | TIM2->CCR3L;
    
    // Switch to falling
    TIM2->SR1 &= ~(1 << 3);
    TIM2->CCER2 = 0x03;  // CC3P=1 (falling), CC3E=1
    
    t = 0;
    while ((TIM2->SR1 & (1 << 3)) == 0) {
        if (++t >= timeout_us) {
            TIM2->CCER2 = 0x01;  // restore rising
            return 0;
        }
    }
    fall = (TIM2->CCR3H << 8) | TIM2->CCR3L;
    TIM2->CCER2 = 0x01;  // restore rising
    return (uint16_t)(fall - rise);
}


uint16_t hcsr04_convert_to_cm(uint16_t time_us) {
    uint32_t dist;
    if (time_us == 0) return 0;
    dist = (uint32_t)time_us / 58;
    return (dist > 400) ? 400 : (uint16_t)dist;
}

uint16_t hcsr04_convert_to_inch(uint16_t time_us) {
    uint32_t dist;
    if (time_us == 0) return 0;
    dist = (uint32_t)time_us / 148; 
    return (dist > 157) ? 157 : (uint16_t)dist;
}