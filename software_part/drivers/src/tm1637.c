#include "tm1637.h"

static const uint8_t seg_map[10] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66,
  0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

void tm_gpio_init(void){
  TM_PORT->ODR |= (TM_CLK_MASK | TM_DIO_MASK);

  /* CLK: output push-pull, fast */
  TM_PORT->DDR |= TM_CLK_MASK;
  TM_PORT->CR1 |= TM_CLK_MASK;
  TM_PORT->CR2 |= TM_CLK_MASK;

  /* DIO: output open-drain, fast */
  TM_PORT->DDR |= TM_DIO_MASK;
  TM_PORT->CR1 &=~ (uint8_t)TM_DIO_MASK;
  TM_PORT->CR2 |= TM_DIO_MASK;
}

void tm_start(void){
  TM_PORT->DDR |= 	TM_DIO_MASK;          /* output */
  TM_PORT->CR1 &=~ (uint8_t)TM_DIO_MASK;/* open-drain */

  TM_PORT->ODR |= 	TM_DIO_MASK;
  TM_PORT->ODR |= 	TM_CLK_MASK;
  _delay_us(2);

  TM_PORT->ODR &=~ (uint8_t)TM_DIO_MASK;
  _delay_us(2);

  TM_PORT->ODR &=~ (uint8_t)TM_CLK_MASK;
  _delay_us(2);
}

void tm_stop(void){
  TM_PORT->DDR |= TM_DIO_MASK;
  TM_PORT->CR1 &=~ (uint8_t)TM_DIO_MASK;

  TM_PORT->ODR &=~ (uint8_t)TM_CLK_MASK;
  _delay_us(2);
  TM_PORT->ODR &=~ (uint8_t)TM_DIO_MASK;
  _delay_us(2);

  TM_PORT->ODR |= TM_CLK_MASK;
  _delay_us(2);

  TM_PORT->ODR |= TM_DIO_MASK;
  _delay_us(2);
}

uint8_t tm_write_byte(uint8_t b){
	int i;
	uint8_t ack;
  for (i = 0; i < 8; ++i){
    TM_PORT->ODR &=~ (uint8_t)TM_CLK_MASK;
    _delay_us(1);

    TM_PORT->DDR |= TM_DIO_MASK;
    TM_PORT->CR1 &=~ (uint8_t)TM_DIO_MASK; /* open-drain */

    if (b & 0x01) TM_PORT->ODR |= TM_DIO_MASK;
    else           TM_PORT->ODR &= (uint8_t)~TM_DIO_MASK;

    _delay_us(3);

    TM_PORT->ODR |= TM_CLK_MASK;
    _delay_us(5);

    b >>= 1;
  }

  TM_PORT->ODR &= (uint8_t)~TM_CLK_MASK;

  TM_PORT->DDR &= (uint8_t)~TM_DIO_MASK; /* input */
  TM_PORT->CR1 |= TM_DIO_MASK;           /* pull-up */

  _delay_us(1);
  TM_PORT->ODR |= TM_CLK_MASK;
  _delay_us(3);

	ack = (TM_PORT->IDR & TM_DIO_MASK) ? 1u : 0u;

  TM_PORT->ODR &=~ (uint8_t)TM_CLK_MASK;
  _delay_us(1);

  TM_PORT->DDR |= TM_DIO_MASK;
  TM_PORT->CR1 &=~ (uint8_t)TM_DIO_MASK;

  return ack ? 1u : 0u;
}

void tm_set_brightness(unsigned short brightness){
  if (brightness > 7) brightness = 7;
  tm_start();
  tm_write_byte(0x88 | (brightness & 0x07));
  tm_stop();
}


static unsigned char char_to_seg(char c){
  if (c >= '0' && c <= '9') return seg_map[c - '0'];
  switch (c){
    case ' ': return 0x00;       
    case '-': return 0x40;       
    case 'A': case 'a': return 0x77; 
    case 'b': return 0x7C;       
    case 'C': case 'c': return 0x58; 
    case 'E': case 'e': return 0x79; 
    case 'F': case 'f': return 0x71; 
    case 'H': case 'h': return 0x76; 
    case 'L': return 0x38;       
    case 'P': case 'p': return 0x73; 
    case 'r': return 0x50;       
    case 'U': case 'u': return 0x3E; 
    case '_': return 0x08;      
    default:  return 0x00;       
  }
}


void tm_display_digits(const char *str){
  int i;
	int index;
	int start;
	char c;
	unsigned char seg;
  int len = 0;
  if (str){
    while (str[len] != '\0' && len < 32) ++len; 
  }
  start = 0;
  if (len > 4) start = len - 4; 
  else start = 0;

  tm_start();
  tm_write_byte(0x40);
  tm_stop();

  tm_start();
  tm_write_byte(0xC0);

  for (i = 0; i < 4; ++i){
    index = start + i;
    if (index < len) c = str[index];
    else c = ' '; 
    seg = char_to_seg(c);
    tm_write_byte(seg);
  }
  tm_stop();
}