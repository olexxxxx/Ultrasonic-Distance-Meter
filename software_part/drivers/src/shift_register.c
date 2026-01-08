#include "shift_register.h"

void shift_reg_init(void) {
    // 1. Скидання бітів у регістрі напрямку (безпека)
    // Це не обов'язково після ресету, але корисно для детермінованості
    SR_PORT->DDR &= ~(SR_DATA_PIN | SR_CLK_PIN | SR_LATCH_PIN);
    
    // 2. Налаштування на вихід (Direction Register = 1)
    SR_PORT->DDR |= (SR_DATA_PIN | SR_CLK_PIN | SR_LATCH_PIN);
    
    // 3. Налаштування режиму Push-Pull (Control Register 1 = 1)
    // Якщо CR1=0, це був би Pseudo-Open-Drain, що вимагало б підтягуючих резисторів.
    SR_PORT->CR1 |= (SR_DATA_PIN | SR_CLK_PIN | SR_LATCH_PIN);
    
    // 4. Налаштування швидкості перемикання (Control Register 2 = 1 -> 10MHz)
    SR_PORT->CR2 |= (SR_DATA_PIN | SR_CLK_PIN | SR_LATCH_PIN);
    
    // 5. Встановлення початкового стану (Low)
    SR_PORT->ODR &= ~(SR_DATA_PIN | SR_CLK_PIN | SR_LATCH_PIN);
}

void shift_reg_send(uint8_t data) {
    uint8_t i;
    uint8_t mask;

    // Початок транзакції: Latch Low
    SR_PORT->ODR &= ~SR_LATCH_PIN;

    // Цикл передачі 8 біт (MSB first - старшим бітом вперед)
    for (i = 0; i < 8; i++) {
        mask = 0x80 >> i; // Зсув маски: 10000000 -> 01000000 -> ...

        // Встановлення лінії даних (DS)
        if (data & mask) {
            SR_PORT->ODR |= SR_DATA_PIN;
        } else {
            SR_PORT->ODR &= ~SR_DATA_PIN;
        }

        // Формування тактового імпульсу (SH_CP)
        SR_PORT->ODR |= SR_CLK_PIN;  // Rising edge -> зсув даних
        SR_PORT->ODR &= ~SR_CLK_PIN; // Falling edge
    }

    // Завершення транзакції: Latch High (ST_CP)
    // Дані переносяться з регістру зсуву в регістр зберігання (на вихід)
    SR_PORT->ODR |= SR_LATCH_PIN; 
    SR_PORT->ODR &= ~SR_LATCH_PIN;
}