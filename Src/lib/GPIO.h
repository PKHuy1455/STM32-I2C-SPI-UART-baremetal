/*
 * GPIO.h
 *
 * Dinh nghia thanh ghi RCC va GPIO dung chung.
 * Cach lam: lay dia chi goc cua khoi ngoai vi (xem trong datasheet,
 * muc "Memory map"), roi cong them offset cua tung thanh ghi (xem trong
 * muc mo ta tung khoi, vi du "RCC register map").
 *
 *  Author: PK Huy
 */
#ifndef SRC_LIB_GPIO_H_
#define SRC_LIB_GPIO_H_

#include <stdint.h>

/* ---- RCC: bat clock cho tung ngoai vi truoc khi dung ---- */
#define RCC_BASE        0x40023800
#define Rcc_AHB1ENR     (*((volatile uint32_t*)(RCC_BASE + 0x30)))
#define Rcc_APB1ENR     (*((volatile uint32_t*)(RCC_BASE + 0x40)))
#define Rcc_APB2ENR     (*((volatile uint32_t*)(RCC_BASE + 0x44)))

/* ---- GPIOA ---- */
#define GPIOA_BASE      0x40020000
#define GpioA_MODER     (*((volatile uint32_t*)(GPIOA_BASE + 0x00)))
#define GpioA_ODR       (*((volatile uint32_t*)(GPIOA_BASE + 0x14)))
#define GpioA_AFRL      (*((volatile uint32_t*)(GPIOA_BASE + 0x20)))
#define GpioA_AFRH      (*((volatile uint32_t*)(GPIOA_BASE + 0x24)))

/* ---- GPIOB ---- */
#define GPIOB_BASE      0x40020400
#define GpioB_MODER     (*((volatile uint32_t*)(GPIOB_BASE + 0x00)))
#define GpioB_OTYPER    (*((volatile uint32_t*)(GPIOB_BASE + 0x04)))
#define GpioB_OSPEEDR   (*((volatile uint32_t*)(GPIOB_BASE + 0x08)))
#define GpioB_PUPDR     (*((volatile uint32_t*)(GPIOB_BASE + 0x0C)))
#define GpioB_AFRL      (*((volatile uint32_t*)(GPIOB_BASE + 0x20)))

#endif /* SRC_LIB_GPIO_H_ */
