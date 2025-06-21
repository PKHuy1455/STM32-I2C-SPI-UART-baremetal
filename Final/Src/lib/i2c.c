#include "i2c.h"
#include "GPIO.h"
#include <stdint.h>
// Hàm cấu hình I2C
void I2C1_Init(void) {
    // Mở clock cho I2C1 và GPIOB
	Rcc_APB1ENR |= (1 << 21);
    Rcc_AHB1ENR |= (1 << 1);

    // Cấu hình các chân SCL (PB6) và SDA (PB7)
    GpioB_MODER |= (0x2 << (2 * 6)) | (0x2 << (2 * 7));  // Chế độ Alternate Function
    GpioB_OTYPER |= (0x1 << 6) | (0x1 << 7);  // Mở kéo xuống (Open Drain)
    GpioB_OSPEEDR |= (0x3 << (2 * 6)) | (0x3 << (2 * 7));  // Tốc độ cao
    GpioB_PUPDR |= (0x1 << (2 * 6)) | (0x1 << (2 * 7));  // Pull-up

    // Cấu hình I2C1
    I2C1_CR2 = 16 ;  // Tần số I2C (16 MHz)
    I2C1_CCR = 80 ;   // Tốc độ I2C (100 kHz)
    I2C1_TRISE = 17;  // Thời gian rise cho I2C

    // Bật I2C
    I2C1_CR1 |= (1 << 0);
}

// Hàm gửi một byte qua I2C
void I2C1_Write(uint8_t addr, uint8_t data) {
    // Gửi địa chỉ slave và bit ghi (0)
	I2C1_DR = addr & 0xFE;
    while (!(I2C1_SR1 & I2C_SR1_TXE));  // Chờ gửi dữ liệu

    // Gửi dữ liệu
    I2C1_DR = data;
    while (!(I2C1_SR1 & I2C_SR1_TXE));  // Chờ gửi dữ liệu
}

// Hàm đọc một byte từ I2C
uint8_t I2C1_Read(uint8_t addr) {
    uint8_t receivedData;

    // Gửi địa chỉ slave và bit đọc (1)
    I2C1_DR = addr | 0x01;
    while (!(I2C1_SR1 & I2C_SR1_RXNE));  // Chờ nhận dữ liệu

    // Đọc dữ liệu nhận được
    receivedData = I2C1_DR;

    return receivedData;
}

// Đọc nhiệt độ từ LM75
float Read_LM75_Temperature(void) {
    uint8_t msb, lsb;
    uint16_t tempRaw;

    // Đọc giá trị nhiệt độ (địa chỉ 0x00)
    I2C1_Write(LM75_ADDR, 0x00);  // Gửi yêu cầu đọc từ thanh ghi nhiệt độ
    msb = I2C1_Read(LM75_ADDR);  // Đọc byte MSB
    lsb = I2C1_Read(LM75_ADDR);  // Đọc byte LSB

    // Kết hợp MSB và LSB thành một giá trị 16-bit
    tempRaw = ((uint16_t)msb << 8) | lsb;

    // LM75 cung cấp nhiệt độ theo đơn vị 0.125°C
    return (float)tempRaw * 0.125;
}
