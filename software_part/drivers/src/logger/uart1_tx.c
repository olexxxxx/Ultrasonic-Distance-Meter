
//==================== INCLUDES ========================  
#include "uart1_tx.h"
#include <string.h>
//==================== DEFINES =========================

#define UART_SR_TXE     0x80    /**< Transmit Data Register Empty */
#define UART_SR_TC      0x40    /**< Transmission Complete */

#define UART_CR1_M      0x10    /**< Word length (0=8bit, 1=9bit) */
#define UART_CR1_PCEN   0x04    /**< Parity control enable */

#define UART_CR2_TEN    0x08    /**< Transmitter enable */
#define UART_CR2_REN    0x04    /**< Receiver enable */
#define UART_CR2_TIEN   0x80    /**< TX interrupt enable */
#define UART_CR2_TCIEN  0x40    /**< TX complete interrupt enable */
#define UART_CR2_RIEN   0x20    /**< RX interrupt enable */

#define UART_CR3_STOP_1BIT  0x00    /**< 1 stop bit */
#define UART_CR3_STOP_2BIT  0x20    /**< 2 stop bits */

//============= STATIC INTERNAL FUNCTIONS PROTOTYPES ==============
static void uart1_tx_byte(uint8_t data);
//============== PUBLIC FUNCTION IMPLEMENTATIONS ==============

void uart1_tx_init(uint32_t baud_rate, uint32_t f_master) {
    uint32_t brr;
    /* PD5: TX as output push-pull */
    GPIOD->DDR |= (1 << 5);     /* Output mode */
    GPIOD->CR1 |= (1 << 5);     /* Push-pull */
    /*
     * Formula: BRR = f_master / baud_rate
     * BRR розбивається на:
     * - BRR1[7:0] = BRR[11:4]
     * - BRR2[7:4] = BRR[15:12]
     * - BRR2[3:0] = BRR[3:0]
     */
    brr = f_master / baud_rate;
    /* BRR2: [15:12] and [3:0] */
    UART1->BRR2 = (uint8_t)((brr & 0x000F) | ((brr >> 8) & 0xF0));
    /* BRR1: [11:4] */
    UART1->BRR1 = (uint8_t)((brr >> 4) & 0xFF);
    /* CR1: 8-bit mode, no parity */
    UART1->CR1 = 0x00;  /* M=0 (8-bit), PCEN=0 (no parity) */
    /* CR2: Enable transmitter only, disable all interrupts */
    UART1->CR2 = UART_CR2_TEN;  /* TEN=1, REN=0, interrupts=0 */
    /* CR3: 1 stop bit */
    UART1->CR3 = UART_CR3_STOP_1BIT;
}



void uart1_tx_buffer(const uint8_t* data, uint16_t length) {
    uint16_t i;
    /* Validate parameters */
    if (data == '\0' || length == 0) {
        return;
    }
    
    /* Send each byte */
    for (i = 0; i < length; i++) {
        uart1_tx_byte(data[i]);
    }
}

void uart1_tx_string(const char* str) {
    /* Validate parameter */
    if (str == NULL) {
        return;
    }
    
    /* Send until null terminator */
    while (*str != '\0') {
        uart1_tx_byte((uint8_t)(*str));
        str++;
    }
}

void uart1_tx_number(int32_t number) {
    char buffer[12];  /* -2147483648 = 11 chars + '\0' */
    uint8_t i;
    /* Handle negative numbers */
    uint8_t is_negative = 0;
    if (number < 0) {
        is_negative = 1;
        number = -number;
    }
    
    /* Convert to string (reverse order) */
    i = 0;
    do {
        buffer[i++] = '0' + (number % 10);
        number /= 10;
    } while (number > 0);
    
    /* Add minus sign */
    if (is_negative) {
        buffer[i++] = '-';
    }
    
    /* Reverse and send */
    while (i > 0) {
        uart1_tx_byte(buffer[--i]);
    }
}

//============= STATIC INTERNAL FUNCTION IMPLEMENTATIONS =======
/**
 * @brief Відправка одного байта по UART
 * 
 * Блокуюча функція - чекає завершення передачі попереднього байта.
 * 
 * @param[in] data Байт для передачі
 * 
 * @note Функція чекає на готовність передавача (TXE flag)
 * 
 * @see uart1_tx_string()
 * @see uart1_tx_buffer()
 */
static void uart1_tx_byte(uint8_t data) {
    /* Wait until Transmit Data Register is Empty */
    while (!(UART1->SR & UART_SR_TXE)) {
        /* Busy wait */
    }
    
    /* Write data to Data Register */
    UART1->DR = data;
}