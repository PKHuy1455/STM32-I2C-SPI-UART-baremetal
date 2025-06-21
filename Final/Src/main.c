#include "lib/spi.h"
#include "lib/uart.h"
#include "lib/i2c.h"
#include "lib/GPIO.h"
#include <stdint.h>
uint32_t pressure;
uint32_t temperature;

int main(void) {

    SPI1_Init();
    I2C1_Init();
    UART1_Init();
    BMP280_Init();
    char buffer[100];
    while (1) {
        // Read raw pressure data
        uint8_t press_msb = BMP280_ReadReg(BMP280_REG_PRESS_MSB);
        uint8_t press_lsb = BMP280_ReadReg(BMP280_REG_PRESS_MSB + 1);
        uint8_t press_xlsb = BMP280_ReadReg(BMP280_REG_PRESS_MSB + 2);

        pressure = (press_msb << 12) | (press_lsb << 4) | (press_xlsb >> 4);

        // Read raw temperature data
        uint8_t temp_msb = BMP280_ReadReg(BMP280_REG_TEMP_MSB);
        uint8_t temp_lsb = BMP280_ReadReg(BMP280_REG_TEMP_MSB + 1);
        uint8_t temp_xlsb = BMP280_ReadReg(BMP280_REG_TEMP_MSB + 2);

        temperature = (temp_msb << 12) | (temp_lsb << 4) | (temp_xlsb >> 4);
        temperature = Read_LM75_Temperature();
        sprintf(buffer, temperature, pressure); // @suppress("Float formatting support")
        USART2_SendString(buffer);
        for (volatile int i = 0; i < 1000000; i++);
    }
}
