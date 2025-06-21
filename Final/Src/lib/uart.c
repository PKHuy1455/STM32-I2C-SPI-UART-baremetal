#include "uart.h"
#include "GPIO.h"
#include <stdint.h>
void UART1_Init(void) {
    // Bật clock cho GPIOA và USART1
    Rcc_AHB1ENR |= (1 << 0);  // Enable clock GPIOA
    Rcc_APB2ENR |= (1 << 4);  // Enable clock USART1

    // Cấu hình chân PA9 (TX) và PA10 (RX)
    GpioA_MODER |= (0b10 << 18) | (0b10 << 20);  // Alternate Function cho PA9, PA10
    GpioA_AFRH |= (0b0111 << 4) | (0b0111 << 8); // AF7 cho USART1

    // Cấu hình USART1
    USART_BRR = (16 << 4) | 5;  // Baud rate 9600 @16MHz
    USART_CR1 |= (1 << 2) | (1 << 3);  // Enable RX, TX
    USART_CR1 |= (1 << 13);            // Enable USART1
}

void USART2_SendChar(char c) {
    while (!(USART_SR & USART_SR_TXE));  // Chờ TXE (có thể gửi dữ liệu)
    USART_DR = c;
}

void USART2_SendString(const char *str) {
    while (*str) {
        USART2_SendChar(*str++);
    }
}
