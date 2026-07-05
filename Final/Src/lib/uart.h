#ifndef UART_H
#define UART_H

#include "stm32f401.h"

/* Function prototypes */
void USART1_Init(uint32_t baud);
void USART1_SendChar(char c);
void USART1_SendString(const char *str);

/* Backward compatibility redirects to USART1 */
#define UART1_Init()            USART1_Init(115200)
#define USART2_Init()           USART1_Init(115200)
#define USART2_SendChar(c)      USART1_SendChar(c)
#define USART2_SendString(str)  USART1_SendString(str)

#endif // UART_H
