/*
 * ES28.h
 *
 *  Created on: Oct 25, 2024
 *
 */

#ifndef ES28_H_
#define ES28_H_

#include "stm32C0xx.h"

/* Returns the number of milliseconds since the device
 * was turned on. */
uint32_t milliseconds( void );

/* Busy wait for the desired number of milliseconds */
void delay_ms( uint32_t ms );

#define GPIO_INPUT 0
#define GPIO_OUTPUT 1
#define GPIO_ALTERNATE 2
#define GPIO_ANALOG 3

#define GPIO_NO_PUP 0
#define GPIO_PULLUP 1
#define GPIO_PULLDOWN 2

#define GPIO_AF0 0
#define GPIO_AF1 1
#define GPIO_AF2 2
#define GPIO_AF3 3
#define GPIO_AF4 4
#define GPIO_AF5 5
#define GPIO_AF6 6
#define GPIO_AF7 7
#define GPIO_AF8 8
#define GPIO_AF9 9
#define GPIO_AF10 10
#define GPIO_AF11 11
#define GPIO_AF12 12
#define GPIO_AF13 13
#define GPIO_AF14 14
#define GPIO_AF15 15

#define EXTI_PA		0
#define EXTI_PB		1
#define EXTI_PC		2
#define EXTI_PD		3

#define TIM_OC1_PWM1 6
#define TIM_OC1_PWM2 7

#define TIM_CC1_OUTPUT 0
#define TIM_CC1_INPUT 1

#endif /* ES28_H_ */
