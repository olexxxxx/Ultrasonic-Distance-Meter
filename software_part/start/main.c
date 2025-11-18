#include "stm8s.h"

/* * HARDWARE DEFINITION
 * Для STM8S103F3P6 "Minimum System Board" світлодіод знаходиться на PB5.
 * Якщо у тебе кастомна плата і там точно PA5 - поверни GPIOA.
 */
#define LED_PORT    GPIOB
#define LED_PIN     5

/* * DELAY FUNCTION
 * Використовує inline assembly nop для запобігання видаленню циклу оптимізатором.
 */
void delay(unsigned long count) {
    while (count--) {
        _asm("nop");
    }
}

int main(void) {
    // 1. CLOCK INITIALIZATION
    // Регістр CKDIVR (Clock Divider Register).
    // 0x00 означає HSI = 16 MHz, CPU = 16 MHz.
    // За замовчуванням там 0x18 (CPU = 2 MHz), що може впливати на розрахунок затримок.
    CLK->CKDIVR = 0x00;

    // 2. GPIO CONFIGURATION
    // DDR (Data Direction): 1 = Output
    LED_PORT->DDR |= (1 << LED_PIN);
    
    // CR1 (Control Register 1): 1 = Push-Pull Output
    // (У режимі Output 0 означає Open Drain, що для LED може не підійти без підтяжки)
    LED_PORT->CR1 |= (1 << LED_PIN);
    
    // CR2 (Control Register 2): 1 = Output speed up to 10 MHz
    LED_PORT->CR2 |= (1 << LED_PIN);

    // 3. MAIN LOOP
    while(1) {
        // Toggle bit using XOR
        LED_PORT->ODR ^= (1 << LED_PIN);
        
        // Затримка. При 16 МГц значення 400000 дасть видиме оку перемикання.
        delay(100000);
    }
}