#ifndef SHIFT_REG_H
#define SHIFT_REG_H

#include "stm8s.h"

/* Конфігурація пінів для 74HC595:
   DS    (Data)  -> PC5
   SH_CP (Clock) -> PC6
   ST_CP (Latch) -> PC7
*/
#define SR_PORT GPIOC
#define SR_DATA_PIN  (1 << 5) // PC5
#define SR_CLK_PIN   (1 << 6) // PC6
#define SR_LATCH_PIN (1 << 7) // PC7

/**
 * @brief Ініціалізація портів GPIO для роботи зі зсувовим регістром.
 * Налаштовує PC5, PC6, PC7 на вихід Push-Pull, High Speed.
 */
void shift_reg_init(void);

/**
 * @brief Відправка байта даних у регістр.
 * Виконує серіалізацію даних (MSB first) та генерацію тактових імпульсів.
 * @param data Байт для відображення (кожен біт відповідає одному виходу Q0-Q7).
 */
void shift_reg_send(uint8_t data);

#endif