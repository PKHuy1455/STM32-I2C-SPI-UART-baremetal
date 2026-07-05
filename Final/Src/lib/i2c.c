#include "i2c.h"
#include "GPIO.h"
#include <stdint.h>

void I2C1_Init(void) {
    /* 1. Enable GPIOB and I2C1 clocks */
    RCC->AHB1ENR |= (1 << 1);  // GPIOB clock enable
    RCC->APB1ENR |= (1 << 21); // I2C1 clock enable

    /* 2. Configure PB6 (SCL) and PB7 (SDA) as Alternate Function Open-Drain with Pull-ups */
    GPIOB->MODER &= ~((3 << (2 * 6)) | (3 << (2 * 7)));
    GPIOB->MODER |= ((2 << (2 * 6)) | (2 << (2 * 7))); // Alternate Function mode

    GPIOB->OTYPER |= (1 << 6) | (1 << 7); // Open-Drain

    GPIOB->OSPEEDR |= ((3 << (2 * 6)) | (3 << (2 * 7))); // High Speed

    GPIOB->PUPDR &= ~((3 << (2 * 6)) | (3 << (2 * 7)));
    GPIOB->PUPDR |= ((1 << (2 * 6)) | (1 << (2 * 7)));  // Pull-up enabled

    /* 3. Assign Alternate Function AF4 (I2C1) to PB6 and PB7 */
    GPIOB->AFR[0] &= ~((0xF << (4 * 6)) | (0xF << (4 * 7)));
    GPIOB->AFR[0] |= ((4 << (4 * 6)) | (4 << (4 * 7))); // AF4 for I2C1

    /* 4. Reset I2C1 to avoid locks */
    I2C1->CR1 |= (1 << 15);  // SWRST = 1
    for (volatile int i = 0; i < 100; i++);
    I2C1->CR1 &= ~(1 << 15); // SWRST = 0

    /* 5. Configure I2C1 Speed (Standard Mode 100 kHz @ 16 MHz HSI Clock) */
    I2C1->CR2 = 16;          // Set peripheral clock to 16 MHz (HSI default)
    I2C1->CCR = 80;          // CCR = 16MHz / (2 * 100kHz) = 80
    I2C1->TRISE = 17;        // TRISE = 16MHz * 1000ns + 1 = 17

    /* 6. Enable I2C1 Peripheral */
    I2C1->CR1 |= (1 << 0);   // PE = 1
}

int I2C1_WriteReg(uint8_t device_addr, uint8_t reg_addr, uint8_t data) {
    /* Send START condition */
    I2C1->CR1 |= (1 << 8);
    while (!(I2C1->SR1 & (1 << 0))); // Wait for SB (Start Bit generated)

    /* Send Slave Address (Write mode: LSB = 0) */
    I2C1->DR = device_addr & 0xFE;
    while (!(I2C1->SR1 & (1 << 1))); // Wait for ADDR (Address matched)
    volatile uint32_t temp = I2C1->SR1;
    temp = I2C1->SR2; // Clear ADDR flag

    /* Send Register Address */
    I2C1->DR = reg_addr;
    while (!(I2C1->SR1 & (1 << 7))); // Wait for TXE

    /* Send Data */
    I2C1->DR = data;
    while (!(I2C1->SR1 & (1 << 7))); // Wait for TXE

    /* Send STOP condition */
    I2C1->CR1 |= (1 << 9);

    return 0;
}

int I2C1_ReadReg(uint8_t device_addr, uint8_t reg_addr, uint8_t *data) {
    return I2C1_ReadBuf(device_addr, reg_addr, data, 1);
}

int I2C1_ReadBuf(uint8_t device_addr, uint8_t reg_addr, uint8_t *buf, uint32_t len) {
    if (len == 0) return -1;

    /* 1. Write the target register address first */
    I2C1->CR1 |= (1 << 8); // Start
    while (!(I2C1->SR1 & (1 << 0))); // Wait for SB

    I2C1->DR = device_addr & 0xFE; // Address + Write
    while (!(I2C1->SR1 & (1 << 1))); // Wait for ADDR
    volatile uint32_t temp = I2C1->SR1;
    temp = I2C1->SR2; // Clear ADDR

    I2C1->DR = reg_addr;
    while (!(I2C1->SR1 & (1 << 7))); // Wait for TXE

    /* 2. Re-start and read buffer */
    I2C1->CR1 |= (1 << 8); // Repeated Start
    while (!(I2C1->SR1 & (1 << 0))); // Wait for SB

    I2C1->DR = device_addr | 0x01; // Address + Read
    while (!(I2C1->SR1 & (1 << 1))); // Wait for ADDR

    if (len == 1) {
        I2C1->CR1 &= ~(1 << 10); // Disable ACK
        temp = I2C1->SR1;
        temp = I2C1->SR2;        // Clear ADDR flag
        
        I2C1->CR1 |= (1 << 9);   // Generate STOP

        while (!(I2C1->SR1 & (1 << 6))); // Wait for RXNE
        buf[0] = I2C1->DR;
    } else if (len == 2) {
        I2C1->CR1 |= (1 << 10);  // Enable ACK
        temp = I2C1->SR1;
        temp = I2C1->SR2;        // Clear ADDR flag

        while (!(I2C1->SR1 & (1 << 6))); // Wait for RXNE (Byte 1 ready)
        buf[0] = I2C1->DR;

        I2C1->CR1 &= ~(1 << 10); // Disable ACK (NACK for last byte)
        I2C1->CR1 |= (1 << 9);   // Generate STOP

        while (!(I2C1->SR1 & (1 << 6))); // Wait for RXNE (Byte 2 ready)
        buf[1] = I2C1->DR;
    } else {
        I2C1->CR1 |= (1 << 10);  // Enable ACK
        temp = I2C1->SR1;
        temp = I2C1->SR2;        // Clear ADDR flag

        for (uint32_t i = 0; i < len - 2; i++) {
            while (!(I2C1->SR1 & (1 << 6)));
            buf[i] = I2C1->DR;
        }

        // Second to last byte
        while (!(I2C1->SR1 & (1 << 6)));
        buf[len - 2] = I2C1->DR;

        I2C1->CR1 &= ~(1 << 10); // Disable ACK
        I2C1->CR1 |= (1 << 9);   // Generate STOP

        // Last byte
        while (!(I2C1->SR1 & (1 << 6)));
        buf[len - 1] = I2C1->DR;
    }

    return 0;
}

float Read_LM75_Temperature(void) {
    uint8_t buf[2];
    /* LM75 temperature data is 11-bit signed format (each step is 0.125°C) */
    if (I2C1_ReadBuf(LM75_ADDR, 0x00, buf, 2) == 0) {
        int16_t tempRaw = (int16_t)((buf[0] << 8) | buf[1]);
        /* Shift right by 5 to extract 11-bit signed value (2's complement) */
        return (float)(tempRaw >> 5) * 0.125f;
    }
    return -999.0f; // Return error code on communication failure
}
