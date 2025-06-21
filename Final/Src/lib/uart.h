
// GPIOA base address and registers


// USART2 base address and registers
#define USART2_BASE     0x40004400
#define USART_SR        (*((volatile uint32_t*)(USART2_BASE + 0x00)))
#define USART_DR        (*((volatile uint32_t*)(USART2_BASE + 0x04)))
#define USART_BRR       (*((volatile uint32_t*)(USART2_BASE + 0x08)))
#define USART_CR1       (*((volatile uint32_t*)(USART2_BASE + 0x0C)))
#define USART_CR2       (*((volatile uint32_t*)(USART1_BASE + 0x10)))
#define USART_CR3       (*((volatile uint32_t*)(USART1_BASE + 0x14)))

#define USART_SR_TXE    (1 << 7)
#define USART_SR_RXNE   (1 << 5)
#ifndef UART_H
#define UART_H

// Hàm khởi tạo UART
void USART2_Init(void);

// Gửi một ký tự qua UART
void USART2_SendChar(char c);

// Gửi một chuỗi qua UART
void USART2_SendString(const char *str);

#endif // UART_H

