/*
 * i2c.c
 *
 * Driver I2C1 don gian, viet thang mot mach cho LM75 (khong tach thanh
 * ham generic Read/Write nhieu lop) de de doc, de nho, de giai thich.
 *
 * Trinh tu I2C bat buoc phai theo (xem Reference Manual RM0368,
 * chuong I2C, phan "Communication flow"):
 *   1. Set bit START trong CR1  -> cho phan cung set co SB
 *   2. Ghi dia chi slave (7-bit + 1 bit doc/ghi) vao DR
 *   3. Cho co ADDR len 1 (dia chi da duoc slave ACK)
 *   4. Doc SR1 roi doc SR2 -> day la cach BAT BUOC de xoa co ADDR
 *   5. Doc/ghi du lieu qua DR
 *   6. Set bit STOP de ket thuc
 *
 * Moi vong while(!(...)) o duoi deu co dem "timeout" don gian: neu cho
 * qua lau ma khong thay co len thi thoat ra, tranh treo cung chuong
 * trinh khi cam bien mat ket noi hoac day dut.
 */
#include "i2c.h"
#include "GPIO.h"

#define I2C_TIMEOUT 50000

void I2C1_Init(void)
{
    /* Buoc 1: bat clock cho I2C1 va GPIOB */
    Rcc_APB1ENR |= (1 << 21);
    Rcc_AHB1ENR |= (1 << 1);

    /* Buoc 2: cau hinh PB6 (SCL) va PB7 (SDA)
     * - Alternate Function (I2C dieu khien chan, khong phai GPIO thuong)
     * - Open-Drain (bat buoc voi I2C: chan chi duoc keo xuong 0, khong
     *   duoc chu dong keo len 1 - keo len la nho dien tro pull-up) */
    GpioB_MODER   |= (0x2 << (2 * 6)) | (0x2 << (2 * 7));
    GpioB_OTYPER  |= (0x1 << 6) | (0x1 << 7);
    GpioB_OSPEEDR |= (0x3 << (2 * 6)) | (0x3 << (2 * 7));
    GpioB_PUPDR   |= (0x1 << (2 * 6)) | (0x1 << (2 * 7));
    GpioB_AFRL    |= (0x4 << (4 * 6)) | (0x4 << (4 * 7)); /* AF4 = I2C1 */

    /* Buoc 3: chon toc do 100kHz voi clock APB1 = 16MHz
     * CCR   = Fpclk1 / (2 * Fscl) = 16000000 / (2*100000) = 80
     * TRISE = Fpclk1(MHz) + 1     = 16 + 1 = 17 */
    I2C1_CR2   = 16;
    I2C1_CCR   = 80;
    I2C1_TRISE = 17;

    /* Buoc 4: bat I2C1 */
    I2C1_CR1 |= (1 << 0);   /* PE = 1 */
}

float Read_LM75_Temperature(void)
{
    uint32_t timeout;
    uint8_t msb, lsb;

    /* ----- Lan 1: gui dia chi thanh ghi can doc (0x00 = thanh ghi nhiet do) ----- */
    I2C1_CR1 |= (1 << 8);                       /* START */
    timeout = I2C_TIMEOUT;
    while (!(I2C1_SR1 & (1 << 0)) && --timeout); /* cho co SB */
    if (timeout == 0) return -999.0f;

    I2C1_DR = LM75_ADDR & 0xFE;                 /* dia chi + bit ghi (0) */
    timeout = I2C_TIMEOUT;
    while (!(I2C1_SR1 & (1 << 1)) && --timeout); /* cho co ADDR */
    if (timeout == 0) return -999.0f;
    (void)I2C1_SR1;
    (void)I2C1_SR2;                              /* doc SR1 roi SR2 de xoa co ADDR */

    I2C1_DR = 0x00;                              /* chon thanh ghi nhiet do */
    timeout = I2C_TIMEOUT;
    while (!(I2C1_SR1 & (1 << 7)) && --timeout); /* cho co TXE (da gui xong) */
    if (timeout == 0) return -999.0f;

    /* ----- Lan 2: Repeated START, doi sang che do doc 2 byte ----- */
    I2C1_CR1 |= (1 << 8);                       /* Repeated START */
    timeout = I2C_TIMEOUT;
    while (!(I2C1_SR1 & (1 << 0)) && --timeout);
    if (timeout == 0) return -999.0f;

    I2C1_DR = LM75_ADDR | 0x01;                 /* dia chi + bit doc (1) */
    timeout = I2C_TIMEOUT;
    while (!(I2C1_SR1 & (1 << 1)) && --timeout);
    if (timeout == 0) return -999.0f;

    I2C1_CR1 |= (1 << 10);                      /* bat ACK, se nhan 2 byte */
    (void)I2C1_SR1;
    (void)I2C1_SR2;                              /* xoa co ADDR */

    timeout = I2C_TIMEOUT;
    while (!(I2C1_SR1 & (1 << 6)) && --timeout); /* cho byte 1 (RXNE) */
    if (timeout == 0) return -999.0f;
    msb = I2C1_DR;

    I2C1_CR1 &= ~(1 << 10);                     /* tat ACK -> bao day la byte cuoi */
    I2C1_CR1 |= (1 << 9);                       /* gui STOP truoc khi doc byte cuoi */

    timeout = I2C_TIMEOUT;
    while (!(I2C1_SR1 & (1 << 6)) && --timeout); /* cho byte 2 */
    if (timeout == 0) return -999.0f;
    lsb = I2C1_DR;

    /* LM75: gia tri 9-bit co dau, buoc nhay 0.5 do C, nam o 7 bit cao cua
     * so 16-bit ghep tu msb/lsb -> dich phai 7 bit roi nhan 0.5 */
    int16_t raw = (int16_t)((msb << 8) | lsb);
    return (float)(raw >> 7) * 0.5f;
}
