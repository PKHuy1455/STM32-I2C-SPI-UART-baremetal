#include <stdio.h>
#include <stdint.h>
#include "lib/spi.h"
#include "lib/uart.h"
#include "lib/i2c.h"
#include "lib/GPIO.h"

int main(void)
{
    SPI1_Init();
    I2C1_Init();
    USART1_Init();

    if (!BMP280_Init()) {
        USART1_SendString("Loi: khong tim thay BMP280, kiem tra day noi SPI/CS\r\n");
    }

    char buffer[100];

    while (1) {
        /* Phai doc nhiet do TRUOC ap suat: cong thuc tinh ap suat can
         * dung gia tri trung gian (t_fine) duoc tinh trong buoc do nhiet do */
        int32_t  temp_c01   = BMP280_ReadTemperature();  /* don vi 0.01 do C */
        uint32_t pressure_q8 = BMP280_ReadPressure();     /* dang Q24.8, /256 ra Pa */

        float lm75_temp = Read_LM75_Temperature();

        /* Tach phan nguyen / phan le de in ra vi khong dung %f duoc
         * (thu vien printf rut gon "nano.specs" khong ho tro in so thuc) */
        int32_t temp_int  = temp_c01 / 100;
        int32_t temp_frac = temp_c01 % 100;

        uint32_t pressure_pa   = pressure_q8 / 256;
        uint32_t pressure_frac = ((pressure_q8 % 256) * 100) / 256;

        if (lm75_temp <= -998.0f) {
            sprintf(buffer, "BMP280: %ld.%02ld C, %lu.%02lu Pa | LM75: loi doc\r\n",
                    (long)temp_int, (long)temp_frac,
                    (unsigned long)pressure_pa, (unsigned long)pressure_frac);
        } else {
            int lm75_int  = (int)lm75_temp;
            int lm75_frac = (int)((lm75_temp - lm75_int) * 100);
            if (lm75_frac < 0) lm75_frac = -lm75_frac;

            sprintf(buffer, "BMP280: %ld.%02ld C, %lu.%02lu Pa | LM75: %d.%02d C\r\n",
                    (long)temp_int, (long)temp_frac,
                    (unsigned long)pressure_pa, (unsigned long)pressure_frac,
                    lm75_int, lm75_frac);
        }
        USART1_SendString(buffer);

        for (volatile int i = 0; i < 1000000; i++);  /* delay don gian */
    }
}
