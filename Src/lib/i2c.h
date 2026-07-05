#ifndef I2C_H
#define I2C_H

#include <stdint.h>

#define I2C1_BASE       0x40005400
#define I2C1_CR1        (*((volatile uint32_t*)(I2C1_BASE + 0x00)))
#define I2C1_CR2        (*((volatile uint32_t*)(I2C1_BASE + 0x04)))
#define I2C1_DR         (*((volatile uint32_t*)(I2C1_BASE + 0x10)))
#define I2C1_SR1        (*((volatile uint32_t*)(I2C1_BASE + 0x14)))
#define I2C1_SR2        (*((volatile uint32_t*)(I2C1_BASE + 0x18)))
#define I2C1_CCR        (*((volatile uint32_t*)(I2C1_BASE + 0x1C)))
#define I2C1_TRISE      (*((volatile uint32_t*)(I2C1_BASE + 0x20)))

#define LM75_ADDR   0x90   /* dia chi 7-bit 0x48, da dich trai 1 bit */

void I2C1_Init(void);

/* Doc nhiet do LM75, tra ve -999.0 neu doc loi (mat ket noi / khong ACK) */
float Read_LM75_Temperature(void);

#endif // I2C_H
