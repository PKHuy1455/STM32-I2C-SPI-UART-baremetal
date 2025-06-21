#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#define I2C1_BASE       0x40005400   // Địa chỉ base của I2C1

// Định nghĩa các thanh ghi I2C1
#define I2C1_CR1        (*((volatile uint32_t*)(I2C1_BASE + 0x00)))  // Control Register 1
#define I2C1_CR2        (*((volatile uint32_t*)(I2C1_BASE + 0x04)))  // Control Register 2
#define I2C1_OAR1       (*((volatile uint32_t*)(I2C1_BASE + 0x08)))  // Own Address Register 1
#define I2C1_OAR2       (*((volatile uint32_t*)(I2C1_BASE + 0x0C)))  // Own Address Register 2
#define I2C1_DR         (*((volatile uint32_t*)(I2C1_BASE + 0x10)))  // Data Register
#define I2C1_SR1        (*((volatile uint32_t*)(I2C1_BASE + 0x14)))  // Status Register 1
#define I2C1_SR2        (*((volatile uint32_t*)(I2C1_BASE + 0x18)))  // Status Register 2
#define I2C1_CCR        (*((volatile uint32_t*)(I2C1_BASE + 0x1C)))  // Clock Control Register
#define I2C1_TRISE      (*((volatile uint32_t*)(I2C1_BASE + 0x20)))  // TRISE Register

#define I2C_SR1_TXE (1 << 7)
#define I2C_SR1_RXNE (1 << 6)

#define LM75_ADDR 0x90// Địa chỉ I2C của LM75 (mặc định: 0x90)
void I2C1_Init(void);// Hàm cấu hình I2C
void I2C1_Write(uint8_t addr, uint8_t data);// Hàm gửi một byte qua I2C
uint8_t I2C1_Read(uint8_t addr);// Hàm đọc một byte từ I2C

float Read_LM75_Temperature(void);// Đọc nhiệt độ từ LM75

#endif // I2C_H
