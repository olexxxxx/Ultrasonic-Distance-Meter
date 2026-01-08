#ifndef __HCSR04_H
#define __HCSR04_H

#include "stm8s.h"
#include "delays.h"
#include "tm1637.h"
#include "adc.h"

#define TIMEOUT_US 30000

void        hcsr04_gpio_init(void);
void        tim2_ch3_enable(void);
void        measure_distance(void);
void        hcsr04_send_trigger(void);
uint16_t    hcsr04_measure_pulse(uint32_t timeout_us);
uint16_t    hcsr04_convert_to_cm(uint16_t time_us);
uint16_t    hcsr04_convert_to_inch(uint16_t time_us);

#endif