#ifndef SPI_H
#define SPI_H

#include <stdint.h>

#define SPI1_BASE       0x40013000
#define SPI1_CR1        (*((volatile uint32_t*)(SPI1_BASE + 0x00)))
#define SPI1_SR         (*((volatile uint32_t*)(SPI1_BASE + 0x08)))
#define SPI1_DR         (*((volatile uint32_t*)(SPI1_BASE + 0x0C)))

#define SPI_SR_TXE   (1 << 1)
#define SPI_SR_RXNE  (1 << 0)

#define BMP280_REG_ID          0xD0
#define BMP280_REG_CTRL_MEAS   0xF4
#define BMP280_REG_CONFIG      0xF5
#define BMP280_REG_PRESS_MSB   0xF7
#define BMP280_REG_TEMP_MSB    0xFA

void SPI1_Init(void);
uint8_t SPI1_TransmitReceive(uint8_t data);

/* Tra ve 1 neu tim thay chip (ID dung), 0 neu khong tim thay */
uint8_t BMP280_Init(void);

/* Doc nhiet do da hieu chinh, don vi 0.01 do C (vi du 2534 nghia la 25.34C) */
int32_t BMP280_ReadTemperature(void);

/* Doc ap suat da hieu chinh, dang so nguyen Q24.8 (tuc la gia tri Pa thuc
 * = ket qua tra ve / 256). Phai goi ReadTemperature() truoc do, vi cong
 * thuc tinh ap suat can dung ket qua trung gian cua buoc do nhiet do. */
uint32_t BMP280_ReadPressure(void);

#endif // SPI_H
