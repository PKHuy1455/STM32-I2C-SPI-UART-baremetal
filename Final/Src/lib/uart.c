#include "uart.h"
#include "GPIO.h"
#include <stdint.h>

void USART1_Init(uint32_t baud) {
    /* 1. Enable GPIOA and USART1 clocks */
    RCC->AHB1ENR |= (1 << 0);  // Enable GPIOA clock
    RCC->APB2ENR |= (1 << 4);  // Enable USART1 clock

    /* 2. Configure PA9 (TX) and PA10 (RX) as Alternate Function Mode */
    GPIOA->MODER &= ~((3 << (2 * 9)) | (3 << (2 * 10)));
    GPIOA->MODER |= ((2 << (2 * 9)) | (2 << (2 * 10))); // Alternate function mode

    /* 3. Map Alternate Function AF7 (USART1) to PA9 & PA10 */
    GPIOA->AFR[1] &= ~((0xF << (4 * (9 - 8))) | (0xF << (4 * (10 - 8))));
    GPIOA->AFR[1] |= ((7 << (4 * (9 - 8))) | (7 << (4 * (10 - 8)))); // AF7

    /* 4. Configure Baud Rate (Assuming 16 MHz HSI Clock) */
    // Divider = f_CK / (16 * baud)
    // For 115200: 16000000 / (16 * 115200) = 8.68
    // Mantissa = 8, Fraction = 0.68 * 16 = 10.88 ~ 11 -> BRR = (8 << 4) | 11 = 0x8B
    if (baud == 115200) {
        USART1->BRR = (8 << 4) | 11;
    } else if (baud == 9600) {
        // Divider = 16000000 / (16 * 9600) = 104.16
        // Mantissa = 104, Fraction = 0.16 * 16 = 2.56 ~ 3 -> BRR = (104 << 4) | 3 = 0x683
        USART1->BRR = (104 << 4) | 3;
    } else {
        // Generic divider calculation
        uint32_t div = 16000000 / baud;
        USART1->BRR = div;
    }

    /* 5. Enable Transmitter, Receiver, and USART1 Peripheral */
    USART1->CR1 = (1 << 13) | (1 << 3) | (1 << 2); // UE, TE, RE
}

void USART1_SendChar(char c) {
    /* Wait until Transmit Data Register is empty (TXE bit 7) */
    while (!(USART1->SR & (1 << 7)));
    USART1->DR = c;
}

void USART1_SendString(const char *str) {
    while (*str) {
        USART1_SendChar(*str++);
    }
}
