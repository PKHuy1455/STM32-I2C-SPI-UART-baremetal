#include "lib/spi.h"
#include "lib/uart.h"
#include "lib/i2c.h"
#include "lib/GPIO.h"
#include <stdint.h>
#include <stdio.h>

int main(void) {
    /* Initialize peripherals */
    SPI1_Init();
    I2C1_Init();
    UART1_Init();  // Redirects internally to USART1_Init(115200)
    BMP280_Init();

    char buffer[128];
    USART2_SendString("=== STM32 Baremetal Sensor Monitor Started ===\r\n");

    while (1) {
        /* 1. Read BMP280 data (burst read 6 bytes for pressure & temperature) */
        uint8_t rawData[6];
        BMP280_ReadBuf(BMP280_REG_PRESS_MSB, rawData, 6);

        int32_t adc_P = ((int32_t)rawData[0] << 12) | ((int32_t)rawData[1] << 4) | (rawData[2] >> 4);
        int32_t adc_T = ((int32_t)rawData[3] << 12) | ((int32_t)rawData[4] << 4) | (rawData[5] >> 4);

        /* Apply Bosch calibration formulas */
        int32_t comp_T = BMP280_Compensate_T(adc_T);    // Output: 0.01 °C units
        uint32_t comp_P = BMP280_Compensate_P(adc_P);   // Output: Q24.8 Pascal units

        float temp_bmp280 = (float)comp_T / 100.0f;
        float press_bmp280 = (float)comp_P / 256.0f / 100.0f; // Convert Pascal to hPa

        /* 2. Read LM75 Temperature via I2C */
        float temp_lm75 = Read_LM75_Temperature();

        /* 3. Safe float printing formatting (no float printf linker flags needed) */
        int32_t temp_bmp_int = (int32_t)temp_bmp280;
        int32_t temp_bmp_frac = (int32_t)((temp_bmp280 - temp_bmp_int) * 100);
        if (temp_bmp_frac < 0) temp_bmp_frac = -temp_bmp_frac;

        int32_t press_bmp_int = (int32_t)press_bmp280;
        int32_t press_bmp_frac = (int32_t)((press_bmp280 - press_bmp_int) * 100);
        if (press_bmp_frac < 0) press_bmp_frac = -press_bmp_frac;

        int32_t temp_lm_int = (int32_t)temp_lm75;
        int32_t temp_lm_frac = (int32_t)((temp_lm75 - temp_lm_int) * 100);
        if (temp_lm_frac < 0) temp_lm_frac = -temp_lm_frac;

        /* Write string to buffer and transmit */
        sprintf(buffer, 
                "BMP280: %ld.%02ld C, %ld.%02ld hPa | LM75: %ld.%02ld C\r\n",
                temp_bmp_int, temp_bmp_frac,
                press_bmp_int, press_bmp_frac,
                temp_lm_int, temp_lm_frac);
        
        USART2_SendString(buffer);

        /* Wait 1 second */
        delay_ms(1000);
    }
}
