#include "spi.h"
#include "GPIO.h"
#include <stdint.h>

/* Define global calibration variable */
BMP280_Calib bmp280_calib;
int32_t bmp280_t_fine = 0;

void delay_ms(uint32_t ms) {
    /* Rough software delay loop (~1ms per iteration @ 16MHz clock) */
    for (volatile uint32_t i = 0; i < ms * 1600; i++);
}

void SPI1_Init(void) {
    /* 1. Enable GPIOA and SPI1 clocks */
    RCC->AHB1ENR |= (1 << 0);   // GPIOA clock enable
    RCC->APB2ENR |= (1 << 12);  // SPI1 clock enable

    /* 2. Configure PA5 (SCK), PA6 (MISO), PA7 (MOSI) as Alternate Function */
    GPIOA->MODER &= ~((3 << (2 * 5)) | (3 << (2 * 6)) | (3 << (2 * 7)));
    GPIOA->MODER |= ((2 << (2 * 5)) | (2 << (2 * 6)) | (2 << (2 * 7))); // AF mode

    /* 3. Assign Alternate Function AF5 (SPI1) to PA5, PA6, PA7 */
    GPIOA->AFR[0] &= ~((0xF << (4 * 5)) | (0xF << (4 * 6)) | (0xF << (4 * 7)));
    GPIOA->AFR[0] |= ((5 << (4 * 5)) | (5 << (4 * 6)) | (5 << (4 * 7))); // AF5

    /* 4. Configure PA4 as Software Chip Select (CS) - Output Mode */
    GPIOA->MODER &= ~(3 << (2 * 4));
    GPIOA->MODER |= (1 << (2 * 4));     // Output mode
    GPIOA->OTYPER &= ~(1 << 4);         // Push-pull
    GPIOA->ODR |= (1 << 4);             // High (deselect BMP280)

    /* 5. Configure SPI1 Peripheral:
       - Master selection (MSTR = 1, bit 2)
       - CPOL = 0, CPHA = 0 (Mode 0)
       - Prescaler: fPCLK / 16 (BR[2:0] = 0b011, bit 3, 4, 5) -> ~1 MHz @ 16 MHz HSI
       - Software Slave Management (SSM = 1, bit 9)
       - Internal Slave Select (SSI = 1, bit 8)
       - SPI Enable (SPE = 1, bit 6)
    */
    SPI1->CR1 = (1 << 2) | (1 << 9) | (1 << 8) | (3 << 3) | (1 << 6);
}

uint8_t SPI1_TransmitReceive(uint8_t data) {
    /* Wait until Transmit Buffer is empty (TXE bit 1) */
    while (!(SPI1->SR & (1 << 1)));
    SPI1->DR = data;

    /* Wait until Receive Buffer is not empty (RXNE bit 0) */
    while (!(SPI1->SR & (1 << 0)));
    return SPI1->DR;
}

void BMP280_WriteReg(uint8_t reg, uint8_t value) {
    GPIOA->ODR &= ~(1 << 4);            // CS low
    SPI1_TransmitReceive(reg & 0x7F);   // MSB = 0 for write
    SPI1_TransmitReceive(value);
    GPIOA->ODR |= (1 << 4);             // CS high
}

uint8_t BMP280_ReadReg(uint8_t reg) {
    uint8_t value;
    GPIOA->ODR &= ~(1 << 4);            // CS low
    SPI1_TransmitReceive(reg | 0x80);   // MSB = 1 for read
    value = SPI1_TransmitReceive(0xFF); // Send dummy byte to clock out data
    GPIOA->ODR |= (1 << 4);             // CS high
    return value;
}

void BMP280_ReadBuf(uint8_t reg, uint8_t *buf, uint32_t len) {
    GPIOA->ODR &= ~(1 << 4);            // CS low
    SPI1_TransmitReceive(reg | 0x80);   // MSB = 1 for read
    for (uint32_t i = 0; i < len; i++) {
        buf[i] = SPI1_TransmitReceive(0xFF);
    }
    GPIOA->ODR |= (1 << 4);             // CS high
}

void BMP280_ReadCalib(void) {
    uint8_t buf[24];
    /* Read 24 bytes calibration starting at 0x88 */
    BMP280_ReadBuf(0x88, buf, 24);

    bmp280_calib.dig_T1 = (buf[1] << 8) | buf[0];
    bmp280_calib.dig_T2 = (buf[3] << 8) | buf[2];
    bmp280_calib.dig_T3 = (buf[5] << 8) | buf[4];

    bmp280_calib.dig_P1 = (buf[7] << 8) | buf[6];
    bmp280_calib.dig_P2 = (buf[9] << 8) | buf[8];
    bmp280_calib.dig_P3 = (buf[11] << 8) | buf[10];
    bmp280_calib.dig_P4 = (buf[13] << 8) | buf[12];
    bmp280_calib.dig_P5 = (buf[15] << 8) | buf[14];
    bmp280_calib.dig_P6 = (buf[17] << 8) | buf[16];
    bmp280_calib.dig_P7 = (buf[19] << 8) | buf[18];
    bmp280_calib.dig_P8 = (buf[21] << 8) | buf[20];
    bmp280_calib.dig_P9 = (buf[23] << 8) | buf[22];
}

void BMP280_Init(void) {
    uint8_t id = BMP280_ReadReg(BMP280_REG_ID);
    if (id != 0x58) {
        /* Lock up if device ID is wrong */
        while (1);
    }

    /* Read calibration values */
    BMP280_ReadCalib();

    /* Configure BMP280:
       - ctrl_meas: Normal Mode, Temp x1, Press x1 oversampling -> 0x27
       - config: Standby 1000ms, Filter off -> 0xA0
    */
    BMP280_WriteReg(BMP280_REG_CTRL_MEAS, 0x27);
    BMP280_WriteReg(BMP280_REG_CONFIG, 0xA0);
}

int32_t BMP280_Compensate_T(int32_t adc_T) {
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)bmp280_calib.dig_T1 << 1))) * ((int32_t)bmp280_calib.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)bmp280_calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)bmp280_calib.dig_T1))) >> 12) * ((int32_t)bmp280_calib.dig_T3)) >> 14;
    bmp280_t_fine = var1 + var2;
    T = (bmp280_t_fine * 5 + 128) >> 8;
    return T;
}

uint32_t BMP280_Compensate_P(int32_t adc_P) {
    int64_t var1, var2, p;
    var1 = ((int64_t)bmp280_t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)bmp280_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)bmp280_calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)bmp280_calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)bmp280_calib.dig_P3) >> 8) + ((var1 * (int64_t)bmp280_calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)bmp280_calib.dig_P1) >> 33;
    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)bmp280_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)bmp280_calib.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)bmp280_calib.dig_P7) << 4);
    return (uint32_t)p;
}
