/*
 * Clock_Init.h
 *
 *  Created on: Dec 20, 2024
 *      Author: PK Huy
 */
#include <stdint.h>
#ifndef SRC_LIB_GPIO_H_
#define SRC_LIB_GPIO_H_

#define RCC_BASE        0x40023800
#define Rcc_AHB1ENR     (*((volatile uint32_t*)(RCC_BASE + 0x30)))
#define Rcc_APB1ENR     (*((volatile uint32_t*)(RCC_BASE + 0x40)))
#define Rcc_APB2ENR     (*((volatile uint32_t*)(RCC_BASE + 0x44)))

#define GPIOA_BASE      0x40020000
#define GpioA_MODER     (*((volatile uint32_t*)(GPIOA_BASE + 0x00)))
#define GpioA_AFRL      (*((volatile uint32_t*)(GPIOA_BASE + 0x20)))
#define GpioA_AFRH      (*((volatile uint32_t*)(GPIOA_BASE + 0x24)))
#define GpioA_ODR     (*((volatile uint32_t*)(GPIOA_BASE + 0x14)))

#define GPIOB_BASE      0x40020400
#define GpioB_MODER     (*((volatile uint32_t*)(GPIOB_BASE + 0x00)))
#define GpioB_AFRL      (*((volatile uint32_t*)(GPIOB_BASE + 0x20)))
#define GpioB_OTYPER    (*((volatile uint32_t*)(GPIOB_BASE + 0x04)))  // Output type register
#define GpioB_OSPEEDR   (*((volatile uint32_t*)(GPIOB_BASE + 0x08)))  // Output speed register
#define GpioB_PUPDR     (*((volatile uint32_t*)(GPIOB_BASE + 0x0C)))  // Pull-up/pull-down register




#endif /* SRC_LIB_GPIO_H_ */
