#ifndef SRC_LIB_GPIO_H_
#define SRC_LIB_GPIO_H_

#include "stm32f401.h"

/* Legacy compatibility macros mapping directly to the new stm32f401 structures */
#define Rcc_AHB1ENR     (RCC->AHB1ENR)
#define Rcc_APB1ENR     (RCC->APB1ENR)
#define Rcc_APB2ENR     (RCC->APB2ENR)

#define GpioA_MODER     (GPIOA->MODER)
#define GpioA_AFRL      (GPIOA->AFR[0])
#define GpioA_AFRH      (GPIOA->AFR[1])
#define GpioA_ODR       (GPIOA->ODR)

#define GpioB_MODER     (GPIOB->MODER)
#define GpioB_AFRL      (GPIOB->AFR[0])
#define GpioB_AFRH      (GPIOB->AFR[1])
#define GpioB_OTYPER    (GPIOB->OTYPER)
#define GpioB_OSPEEDR   (GPIOB->OSPEEDR)
#define GpioB_PUPDR     (GPIOB->PUPDR)

#endif /* SRC_LIB_GPIO_H_ */
