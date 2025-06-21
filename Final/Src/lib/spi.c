#include "spi.h"
#include "GPIO.h"
#include <stdint.h>

void SPI1_Init(void) {
    // Enable GPIOA and SPI1 clocks
	Rcc_AHB1ENR |= (1 << 0);
	Rcc_APB2ENR |= (1 << 12);

    // Configure SCK, MISO, MOSI as alternate function
	GpioA_MODER |= (0b10 << 10) | (0b10 << 12) | (0b10 << 14);  // Alternate Function cho PA5, PA6, PA7
	GpioA_AFRL  |= (0b0101 << 20) | (0b0101 << 24) | (0b0101 << 28); // AF5 cho SPI1

	// Configure CS as output
	GpioA_MODER |= (1 << (4 * 2));  // Output mode for CS
	GpioA_ODR |= (1 << 4);          // Set CS high (not selected)


    // Configure SPI1 in master mode, 8-bit data format, fPCLK/16
    SPI1_CR1 = (1 << 2)     // Master mode
               | (1 << 9)    // 8-bit data frame format
               | (1 << 8)    // SPI enable
               | (1 << 6);   // SCK idle low

}

uint8_t SPI1_TransmitReceive(uint8_t data) {
    // Wait until TXE is set (Transmit buffer empty)
    while (!(SPI1_SR & SPI_SR_TXE));

    // Send data
    SPI1_DR = data;

    // Wait until RXNE is set (Receive buffer not empty)
    while (!(SPI1_SR & SPI_SR_RXNE));

    // Return received data (from data register, not status register)
    return SPI1_DR;
}


void BMP280_WriteReg(uint8_t reg, uint8_t value) {
    // Pull CS low
	GpioA_ODR &= ~(1 << 4);

    // Send register address (with write bit 0)
    SPI1_TransmitReceive(reg & 0x7F);

    // Send value
    SPI1_TransmitReceive(value);

    // Pull CS high
    GpioA_ODR |= (1 << 4);
}

uint8_t BMP280_ReadReg(uint8_t reg) {
    uint8_t value;

    // Pull CS low
    GpioA_ODR &= ~(1 << 4);

    // Send register address (with read bit 1)
    SPI1_TransmitReceive(reg | 0x80);

    // Read value from SPI data register
    value = SPI1_TransmitReceive(0xFF);

    // Pull CS high
    GpioA_ODR |= (1 << 4);

    return value;
}


void BMP280_Init(void) {
    // Check device ID
    uint8_t id = BMP280_ReadReg(BMP280_REG_ID);
    if (id != 0x58) {
        // Error: BMP280 not found
        while (1);
    }

    // Configure BMP280
    BMP280_WriteReg(BMP280_REG_CTRL_MEAS, 0x27); // Normal mode, temp and pressure oversampling x1
    BMP280_WriteReg(BMP280_REG_CONFIG, 0xA0);    // Standby 1000ms, filter off
}
