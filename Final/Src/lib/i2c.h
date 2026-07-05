#ifndef I2C_H
#define I2C_H

#include "stm32f401.h"

#define LM75_ADDR       0x90    // LM75 standard 7-bit address is 0x48 (0x90 when shifted left by 1)

/* Function Prototypes */
void I2C1_Init(void);
int I2C1_WriteReg(uint8_t device_addr, uint8_t reg_addr, uint8_t data);
int I2C1_ReadReg(uint8_t device_addr, uint8_t reg_addr, uint8_t *data);
int I2C1_ReadBuf(uint8_t device_addr, uint8_t reg_addr, uint8_t *buf, uint32_t len);

/* Peripheral functions */
float Read_LM75_Temperature(void);

/* Backward compatibility redirects if needed */
#define I2C1_Write(addr, data)  I2C1_WriteReg(addr, 0x00, data)
#define I2C1_Read(addr)         ({ uint8_t d; I2C1_ReadReg(addr, 0x00, &d); d; })

#endif // I2C_H
