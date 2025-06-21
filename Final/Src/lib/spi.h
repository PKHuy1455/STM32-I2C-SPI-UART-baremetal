#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#define SPI1_BASE       0x40013000  // Địa chỉ base của SPI1

// Định nghĩa các thanh ghi SPI1
#define SPI1_CR1        (*((volatile uint32_t*)(SPI1_BASE + 0x00)))  // Control Register 1
#define SPI1_CR2        (*((volatile uint32_t*)(SPI1_BASE + 0x04)))  // Control Register 2
#define SPI1_SR         (*((volatile uint32_t*)(SPI1_BASE + 0x08)))  // Status Register
#define SPI1_DR         (*((volatile uint32_t*)(SPI1_BASE + 0x0C)))  // Data Register
#define SPI1_CRCPR      (*((volatile uint32_t*)(SPI1_BASE + 0x10)))  // CRC Polynomial Register
#define SPI1_RXCRCR     (*((volatile uint32_t*)(SPI1_BASE + 0x14)))  // RX CRC Register
#define SPI1_TXCRCR     (*((volatile uint32_t*)(SPI1_BASE + 0x18)))  // TX CRC Register
#define SPI1_I2SCFGR    (*((volatile uint32_t*)(SPI1_BASE + 0x1C)))  // I2S Configuration Register
#define SPI1_I2SPR      (*((volatile uint32_t*)(SPI1_BASE + 0x20)))  // I2S Prescaler


// Define SPI pins for BMP280
#define SPI_CS_PIN      4
#define SPI_CS_PORT     GPIOA
#define SPI_SCK_PIN     5
#define SPI_MISO_PIN    6
#define SPI_MOSI_PIN    7
#define SPI_PORT        GPIOA

#define SPI_SR_TXE  (1 << 1)
#define SPI_SR_RXNE  (1 << 0)

#define BMP280_REG_ID            0xD0
#define BMP280_REG_CTRL_MEAS     0xF4
#define BMP280_REG_CONFIG        0xF5
#define BMP280_REG_PRESS_MSB     0xF7
#define BMP280_REG_TEMP_MSB      0xFA

void delay(uint32_t count);
void SPI1_Init(void);
uint8_t SPI1_TransmitReceive(uint8_t data);
void BMP280_WriteReg(uint8_t reg, uint8_t value);
uint8_t BMP280_ReadReg(uint8_t reg);
void BMP280_Init(void);

#endif // SPI_H
