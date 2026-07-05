#ifndef SPI_H
#define SPI_H

#include "stm32f401.h"

/* SPI Pins Mapping for BMP280 on Black Pill Board */
#define SPI_CS_PIN      4
#define SPI_SCK_PIN     5
#define SPI_MISO_PIN    6
#define SPI_MOSI_PIN    7

/* BMP280 Registers */
#define BMP280_REG_ID            0xD0
#define BMP280_REG_CTRL_MEAS     0xF4
#define BMP280_REG_CONFIG        0xF5
#define BMP280_REG_PRESS_MSB     0xF7
#define BMP280_REG_TEMP_MSB      0xFA

/* BMP280 Calibration Struct */
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} BMP280_Calib;

extern BMP280_Calib bmp280_calib;
extern int32_t bmp280_t_fine;

/* Function Prototypes */
void delay_ms(uint32_t ms);
void SPI1_Init(void);
uint8_t SPI1_TransmitReceive(uint8_t data);
void BMP280_WriteReg(uint8_t reg, uint8_t value);
uint8_t BMP280_ReadReg(uint8_t reg);
void BMP280_ReadBuf(uint8_t reg, uint8_t *buf, uint32_t len);
void BMP280_Init(void);

/* Optimized Calibration / Compensation Formulas */
int32_t BMP280_Compensate_T(int32_t adc_T);
uint32_t BMP280_Compensate_P(int32_t adc_P);

#endif // SPI_H
