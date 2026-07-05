/*
 * uart.c
 *
 * Driver USART1, toc do co dinh 9600 baud (thay vi tinh cong thuc tong quat
 * cho baud bat ky - de don gian va de giai thich khi bi hoi).
 */
#include "uart.h"
#include "GPIO.h"

void USART1_Init(void)
{
    Rcc_AHB1ENR |= (1 << 0);   /* bat clock GPIOA */
    Rcc_APB2ENR |= (1 << 4);   /* bat clock USART1 */

    /* PA9 = TX, PA10 = RX -> Alternate Function AF7 (USART1) */
    GpioA_MODER |= (0b10 << 18) | (0b10 << 20);
    GpioA_AFRH  |= (0b0111 << 4) | (0b0111 << 8);

    /* Tinh BRR cho 9600 baud voi clock APB2 = 16MHz (HSI mac dinh):
     *   USARTDIV = Fck / (16 * baud) = 16000000 / (16*9600) = 104.16
     *   Phan nguyen (mantissa) = 104  -> nam o bit [15:4]
     *   Phan le: 0.16 * 16 = 2.6 ~ lam tron thanh 3 -> nam o bit [3:0]
     *   BRR = (104 << 4) | 3 */
    USART1_BRR = (104 << 4) | 3;

    USART1_CR1 |= (1 << 2) | (1 << 3);  /* bat RX, TX */
    USART1_CR1 |= (1 << 13);            /* bat USART1 */
}

void USART1_SendChar(char c)
{
    while (!(USART1_SR & USART_SR_TXE));  /* cho gui xong ky tu truoc */
    USART1_DR = c;
}

void USART1_SendString(const char *str)
{
    while (*str) {
        USART1_SendChar(*str++);
    }
}
