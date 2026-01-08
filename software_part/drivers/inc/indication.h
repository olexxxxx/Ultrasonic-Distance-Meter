#ifndef __INDICATION_H
#define __INDICATION_H

#include "stm8s.h"
#include "adc.h"
#include "hc_sr04.h"
#include "shift_register.h"

void indication_init(void);

void start_indicate(uint8_t units, uint16_t threshold, uint16_t raw_distance);


#endif