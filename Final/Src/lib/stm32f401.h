#ifndef STM32F401_H
#define STM32F401_H

#include <stdint.h>

/* ========================================================================= */
/*                   Peripheral Register Mapping Structures                  */
/* ========================================================================= */

typedef struct {
    volatile uint32_t CR;            /* 0x00 */
    volatile uint32_t PLLCFGR;       /* 0x04 */
    volatile uint32_t CFGR;          /* 0x08 */
    volatile uint32_t CIR;           /* 0x0C */
    volatile uint32_t AHB1RSTR;      /* 0x10 */
    volatile uint32_t AHB2RSTR;      /* 0x14 */
    uint32_t Reserved0[2];
    volatile uint32_t APB1RSTR;      /* 0x20 */
    volatile uint32_t APB2RSTR;      /* 0x24 */
    uint32_t Reserved1[2];
    volatile uint32_t AHB1ENR;       /* 0x30 */
    volatile uint32_t AHB2ENR;       /* 0x34 */
    uint32_t Reserved2[2];
    volatile uint32_t APB1ENR;       /* 0x40 */
    volatile uint32_t APB2ENR;       /* 0x44 */
    uint32_t Reserved3[2];
    volatile uint32_t AHB1LPENR;     /* 0x50 */
    volatile uint32_t AHB2LPENR;     /* 0x54 */
    uint32_t Reserved4[2];
    volatile uint32_t APB1LPENR;     /* 0x60 */
    volatile uint32_t APB2LPENR;     /* 0x64 */
    uint32_t Reserved5[2];
    volatile uint32_t BDCR;          /* 0x70 */
    volatile uint32_t CSR;           /* 0x74 */
    uint32_t Reserved6[2];
    volatile uint32_t SSCGR;         /* 0x80 */
    volatile uint32_t PLLI2SCFGR;    /* 0x84 */
} RCC_TypeDef;

typedef struct {
    volatile uint32_t MODER;         /* 0x00 */
    volatile uint32_t OTYPER;        /* 0x04 */
    volatile uint32_t OSPEEDR;       /* 0x08 */
    volatile uint32_t PUPDR;         /* 0x0C */
    volatile uint32_t IDR;           /* 0x10 */
    volatile uint32_t ODR;           /* 0x14 */
    volatile uint32_t BSRR;          /* 0x18 */
    volatile uint32_t LCKR;          /* 0x1C */
    volatile uint32_t AFR[2];        /* 0x20 - 0x24 (AFRL and AFRH) */
} GPIO_TypeDef;

typedef struct {
    volatile uint32_t SR;            /* 0x00 */
    volatile uint32_t DR;            /* 0x04 */
    volatile uint32_t BRR;           /* 0x08 */
    volatile uint32_t CR1;           /* 0x0C */
    volatile uint32_t CR2;           /* 0x10 */
    volatile uint32_t CR3;           /* 0x14 */
    volatile uint32_t GTPR;          /* 0x18 */
} USART_TypeDef;

typedef struct {
    volatile uint32_t CR1;           /* 0x00 */
    volatile uint32_t CR2;           /* 0x04 */
    volatile uint32_t OAR1;          /* 0x08 */
    volatile uint32_t OAR2;          /* 0x0C */
    volatile uint32_t DR;            /* 0x10 */
    volatile uint32_t SR1;           /* 0x14 */
    volatile uint32_t SR2;           /* 0x18 */
    volatile uint32_t CCR;           /* 0x1C */
    volatile uint32_t TRISE;         /* 0x20 */
    volatile uint32_t FLTR;          /* 0x24 */
} I2C_TypeDef;

typedef struct {
    volatile uint32_t CR1;           /* 0x00 */
    volatile uint32_t CR2;           /* 0x04 */
    volatile uint32_t SR;            /* 0x08 */
    volatile uint32_t DR;            /* 0x0C */
    volatile uint32_t CRCPR;         /* 0x10 */
    volatile uint32_t RXCRCR;        /* 0x14 */
    volatile uint32_t TXCRCR;        /* 0x18 */
    volatile uint32_t I2SCFGR;       /* 0x1C */
    volatile uint32_t I2SPR;         /* 0x20 */
} SPI_TypeDef;

/* ========================================================================= */
/*                         Peripheral Base Addresses                         */
/* ========================================================================= */

#define RCC_BASE      0x40023800
#define GPIOA_BASE    0x40020000
#define GPIOB_BASE    0x40020400
#define USART1_BASE   0x40011000
#define USART2_BASE   0x40004400
#define I2C1_BASE     0x40005400
#define SPI1_BASE     0x40013000

/* ========================================================================= */
/*                          Peripheral Definitions                           */
/* ========================================================================= */

#define RCC           ((RCC_TypeDef *) RCC_BASE)
#define GPIOA         ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB         ((GPIO_TypeDef *) GPIOB_BASE)
#define USART1        ((USART_TypeDef *) USART1_BASE)
#define USART2        ((USART_TypeDef *) USART2_BASE)
#define I2C1          ((I2C_TypeDef *) I2C1_BASE)
#define SPI1          ((SPI_TypeDef *) SPI1_BASE)

#endif /* STM32F401_H */
