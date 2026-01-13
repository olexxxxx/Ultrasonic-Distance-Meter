
#include "logger.h"
#include "logger_config.h"

#ifdef LOGGER_UART_ENABLE
    #include "uart1_tx.h"
    #include <string.h>
#endif

//============ PUBLIC FUNCTION IMPLEMENTATIONS =========

void initialize_logger(void) {
#ifdef LOGGER_UART_ENABLE
    /* Ініціалізація UART1 TX для логування */
    uart1_tx_init(UART_CURRENT_SPEED, UART_SYSTEM_CLOCK);
#else
    /* Logger disabled - do nothing */
#endif
}

void write_message_in_logger(const char* msg) {
#ifdef LOGGER_UART_ENABLE
    /* Send message only if logger enabled */
    if (msg != NULL) {
        uart1_tx_string(msg);
        uart1_tx_string("\r\n");  /* Add newline for readability */
    }
#else
    (void)msg;  /* Avoid unused parameter warning */
#endif
}

void write_number_in_logger(int32_t number) {
#ifdef LOGGER_UART_ENABLE
    uart1_tx_number(number);
    uart1_tx_string("\r\n");
#else
    (void)number;
#endif
}