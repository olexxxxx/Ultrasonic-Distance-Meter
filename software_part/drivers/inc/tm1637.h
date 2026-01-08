#ifndef TM1637_H
#define TM1637_H


#include "stm8s.h" 
#include "delays.h"



#define TM_PORT        GPIOC
#define TM_DIO_PIN     4U
#define TM_CLK_PIN     3U
#define TM_DIO_MASK    ((unsigned char)(1U << TM_DIO_PIN))
#define TM_CLK_MASK    ((unsigned char)(1U << TM_CLK_PIN))


void tm_gpio_init(void);
void tm_start(void);
void tm_stop(void);

unsigned char tm_write_byte(unsigned char b);

void tm_set_brightness(unsigned short brightness); 
void tm_display_digits(const char *str);


#endif /* TM1637_STM8_H */