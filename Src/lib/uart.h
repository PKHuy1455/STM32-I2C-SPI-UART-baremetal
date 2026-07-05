#ifndef UART_H
#define UART_H

#include <stdint.h>

#define USART1_BASE     0x40011000
#define USART1_SR       (*((volatile uint32_t*)(USART1_BASE + 0x00)))
#define USART1_DR       (*((volatile uint32_t*)(USART1_BASE + 0x04)))
#define USART1_BRR      (*((volatile uint32_t*)(USART1_BASE + 0x08)))
#define USART1_CR1      (*((volatile uint32_t*)(USART1_BASE + 0x0C)))

#define USART_SR_TXE    (1 << 7)

void USART1_Init(void);
void USART1_SendChar(char c);
void USART1_SendString(const char *str);

#endif // UART_H
