#include "sys_clk.h"
/**

  * @brief CLK Initialization Function

  * @param None

  * @retval None

  * @par HSI 16Mhz

  */

void enable_system_clock(void)
{
    /* Enable HSI and LSI */
    CLK->ICKR |= 0x01; // HSIEN
    /* Wait until HSI ready (HSIRDY bit) */
    while ((CLK->ICKR & 0x02) == 0) {;} // HSIRDY

    CLK->CKDIVR &= (uint8_t)~(0x18 | 0x07);

}