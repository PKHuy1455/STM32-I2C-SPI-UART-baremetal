/*
 * spi.c
 *
 * Driver SPI1 + logic doc cam bien BMP280 gop chung 1 file (giong cach
 * to chuc ban dau), thay vi tach rieng lop "driver bus" va lop "cam bien".
 *
 * BMP280 xuat du lieu tho (raw ADC) - KHONG phai gia tri vat ly that.
 * Datasheet Bosch yeu cau doc them 12 he so hieu chinh rieng cua tung
 * chip (nap san trong nha may) roi ap vao cong thuc co san trong datasheet
 * moi ra duoc do C / Pa dung. Cong thuc nay la cong thuc dinh san bat buoc
 * phai dung theo, khong phai tu suy luan/tu chung minh ra.
 */
#include "spi.h"
#include "GPIO.h"

#define BMP280_CS_PIN 4

static uint16_t dig_T1;
static int16_t  dig_T2, dig_T3;
static uint16_t dig_P1;
static int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
static int32_t  t_fine;   /* gia tri trung gian, tinh o buoc do nhiet do,
                              dung lai o buoc do ap suat */

void SPI1_Init(void)
{
    Rcc_AHB1ENR |= (1 << 0);
    Rcc_APB2ENR |= (1 << 12);

    /* PA5=SCK, PA6=MISO, PA7=MOSI -> Alternate Function AF5 (SPI1) */
    GpioA_MODER |= (0b10 << 10) | (0b10 << 12) | (0b10 << 14);
    GpioA_AFRL  |= (0b0101 << 20) | (0b0101 << 24) | (0b0101 << 28);

    /* PA4 = chan CS, dieu khien tay (khong dung NSS tu dong cua phan cung) */
    GpioA_MODER |= (1 << (BMP280_CS_PIN * 2));
    GpioA_ODR   |= (1 << BMP280_CS_PIN);   /* CS = 1 (chua chon chip) */

    /* Master mode, quan ly CS bang phan mem (SSM=1, SSI=1), bat SPI (SPE=1).
     * CPOL=0, CPHA=0 (mac dinh, khong can set) = SPI Mode 0, dung cho BMP280. */
    SPI1_CR1 = (1 << 2)     /* MSTR */
             | (1 << 9)     /* SSM  */
             | (1 << 8)     /* SSI  */
             | (1 << 6);    /* SPE  */
}

uint8_t SPI1_TransmitReceive(uint8_t data)
{
    while (!(SPI1_SR & SPI_SR_TXE));   /* cho gui xong */
    SPI1_DR = data;

    while (!(SPI1_SR & SPI_SR_RXNE));  /* cho nhan xong */
    return SPI1_DR;
}

static uint8_t BMP280_ReadReg(uint8_t reg)
{
    uint8_t value;
    GpioA_ODR &= ~(1 << BMP280_CS_PIN);       /* CS xuong 0: bat dau giao tiep */
    SPI1_TransmitReceive(reg | 0x80);          /* bit 7 = 1 nghia la doc */
    value = SPI1_TransmitReceive(0xFF);        /* gui byte "rac" de lay xung clock */
    GpioA_ODR |= (1 << BMP280_CS_PIN);        /* CS len 1: ket thuc */
    return value;
}

static void BMP280_WriteReg(uint8_t reg, uint8_t value)
{
    GpioA_ODR &= ~(1 << BMP280_CS_PIN);
    SPI1_TransmitReceive(reg & 0x7F);          /* bit 7 = 0 nghia la ghi */
    SPI1_TransmitReceive(value);
    GpioA_ODR |= (1 << BMP280_CS_PIN);
}

static void BMP280_ReadCalibration(void)
{
    /* Doc lan luot 24 byte he so hieu chinh, tu dia chi 0x88 den 0x9F.
     * Moi he so la so 16-bit, byte thap doc truoc (little-endian). */
    uint8_t b[24];
    for (int i = 0; i < 24; i++) {
        b[i] = BMP280_ReadReg(0x88 + i);
    }
    dig_T1 = (uint16_t)(b[0]  | (b[1]  << 8));
    dig_T2 = (int16_t) (b[2]  | (b[3]  << 8));
    dig_T3 = (int16_t) (b[4]  | (b[5]  << 8));
    dig_P1 = (uint16_t)(b[6]  | (b[7]  << 8));
    dig_P2 = (int16_t) (b[8]  | (b[9]  << 8));
    dig_P3 = (int16_t) (b[10] | (b[11] << 8));
    dig_P4 = (int16_t) (b[12] | (b[13] << 8));
    dig_P5 = (int16_t) (b[14] | (b[15] << 8));
    dig_P6 = (int16_t) (b[16] | (b[17] << 8));
    dig_P7 = (int16_t) (b[18] | (b[19] << 8));
    dig_P8 = (int16_t) (b[20] | (b[21] << 8));
    dig_P9 = (int16_t) (b[22] | (b[23] << 8));
}

uint8_t BMP280_Init(void)
{
    uint8_t id = BMP280_ReadReg(BMP280_REG_ID);
    if (id != 0x58) {
        return 0;   /* khong tim thay chip - bao loi, khong treo cung */
    }

    BMP280_ReadCalibration();

    BMP280_WriteReg(BMP280_REG_CTRL_MEAS, 0x27); /* normal mode, oversampling x1 */
    BMP280_WriteReg(BMP280_REG_CONFIG, 0xA0);    /* standby 1000ms, filter off */
    return 1;
}

/* Ghep 3 byte MSB/LSB/XLSB thanh 1 so ADC tho 20-bit */
static uint32_t BMP280_ReadRaw(uint8_t msb_reg)
{
    uint8_t msb  = BMP280_ReadReg(msb_reg);
    uint8_t lsb  = BMP280_ReadReg(msb_reg + 1);
    uint8_t xlsb = BMP280_ReadReg(msb_reg + 2);
    return ((uint32_t)msb << 12) | ((uint32_t)lsb << 4) | (xlsb >> 4);
}

/* Cong thuc bu nhiet do - chep dung nguyen tu datasheet Bosch BMP280,
 * muc "Compensation formula" (ban 32-bit). Tra ve don vi 0.01 do C. */
int32_t BMP280_ReadTemperature(void)
{
    int32_t adc_T = (int32_t)BMP280_ReadRaw(BMP280_REG_TEMP_MSB);

    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * (int32_t)dig_T2) >> 11;
    int32_t var2 = (((((adc_T >> 4) - (int32_t)dig_T1) * ((adc_T >> 4) - (int32_t)dig_T1)) >> 12)
                    * (int32_t)dig_T3) >> 14;
    t_fine = var1 + var2;

    return (t_fine * 5 + 128) >> 8;
}

/* Cong thuc bu ap suat - cung chep tu datasheet Bosch (ban 64-bit).
 * Dung so int64_t vi cong thuc can do chinh xac cao, tranh tran so. */
uint32_t BMP280_ReadPressure(void)
{
    int32_t adc_P = (int32_t)BMP280_ReadRaw(BMP280_REG_PRESS_MSB);

    int64_t var1 = (int64_t)t_fine - 128000;
    int64_t var2 = var1 * var1 * (int64_t)dig_P6;
    var2 += (var1 * (int64_t)dig_P5) << 17;
    var2 += ((int64_t)dig_P4) << 35;
    var1  = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
    var1  = (((((int64_t)1) << 47) + var1) * (int64_t)dig_P1) >> 33;

    if (var1 == 0) return 0;   /* tranh chia cho 0 */

    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = ((int64_t)dig_P9 * (p >> 13) * (p >> 13)) >> 25;
    var2 = ((int64_t)dig_P8 * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);

    return (uint32_t)p;   /* dang Q24.8: chia cho 256 se ra don vi Pa that */
}
