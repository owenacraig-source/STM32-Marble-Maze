/*Owen Craig
 * 5/8/25
 *
 * Driver for tb6612
 * PA5 to IN1
 * PA6 to IN2
 * PA7 to PWM
 *
 * LAB 6
 */

#define PSC_FACTOR	6		    // 12 MHz / 6 = 2 MHz
#define PWM_TIMER_MAX	  1250		// 2 MHz / 1250 = 1.6 kHz with prescale 6

#include "ES28.h"
#include <tb6612.h>

void motor_init(void){


	//Setting pins PA5 and PA6 as IN1 and IN2
	RCC->IOPENR |=  RCC_IOPENR_GPIOAEN;

	// Set PA5 and PA6 as output pins
	GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk;
	GPIOA->MODER |= GPIO_OUTPUT << GPIO_MODER_MODE5_Pos;
	GPIOA->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOA->MODER |= GPIO_OUTPUT << GPIO_MODER_MODE6_Pos;


	// Setting PA7 as PWM pin
	// Set PA7 in alternate function mode (10)
	GPIOA->MODER &= ~GPIO_MODER_MODE7_Msk;
	GPIOA->MODER |= GPIO_ALTERNATE << GPIO_MODER_MODE7_Pos;

	// Set PA7 alternate function type (AF4, 0100)
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL7_Msk;
	GPIOA->AFR[0] |= GPIO_AF4 << GPIO_AFRL_AFSEL7_Pos;

	// Enable clock access to Timer 14
	RCC->APBENR2 |= RCC_APBENR2_TIM14EN;

	// Set prescaler
	TIM14->PSC = PSC_FACTOR-1;

	// Set autoreload value - this is the larger timer
	TIM14->ARR = PWM_TIMER_MAX-1;

	// Set the compare register value - start with a speed of 0
	// this is the duration of the high pulse, the smaller timer
	TIM14->CCR1 = 0;

	// Set pwm mode 1 (0110): pin is high up to capture value, then goes low
	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
	TIM14->CCMR1 |= TIM_OC1_PWM1 << TIM_CCMR1_OC1M_Pos;

	// Make sure timer is in output mode; this is default value
	TIM14->CCMR1 &= ~TIM_CCMR1_CC1S_Msk;
	TIM14->CCMR1 |= TIM_CC1_OUTPUT << TIM_CCMR1_CC1S_Pos;

	// Enable TIM14_CH1 in output mode
	TIM14->CCER |= TIM_CCER_CC1E;

	// Generate an update event - this clears counter, prescaler counter, updates registers
	TIM14->EGR |= TIM_EGR_UG;

	// Enable timer
	TIM14->CR1 |= TIM_CR1_CEN;
}


void motor_mode(uint8_t mode){
	//PA5 to IN1
	//PA6 to IN2

	// mode = 0 STOP: IN1 = 0, IN2 = 0
	// mode = 1 CW:		IN1 = 1 IN2 = 0
	// mode = 2 CCW: IN1 = 0 IN2 = 1
	// mode = 3 BREAK: IN1 =1 IN2 = 1


	switch(mode){
	case 0:
		//IN1 =0, IN2 = 0
		GPIOA->ODR &= ~GPIO_ODR_OD5;
		GPIOA->ODR &= ~GPIO_ODR_OD6;
		break;
	case 1:
		GPIOA->ODR &= ~GPIO_ODR_OD5; // IN1 =0
		GPIOA->ODR |= GPIO_ODR_OD6; // IN2 = 1
		break;
	case 2:
		GPIOA->ODR |= GPIO_ODR_OD5; // IN1 =1
		GPIOA->ODR &= ~GPIO_ODR_OD6; // IN2 = 0
		break;
	case 3:
		GPIOA->ODR |= GPIO_ODR_OD5; // IN1 =1
		GPIOA->ODR |= GPIO_ODR_OD6; // IN2 = 1
		break;
	}
}

void motor_speed(uint16_t pwm_value){
	// set compare value to argument of function
	// argument must be between 0 and 1249

	if (pwm_value>PWM_TIMER_MAX-1)
			pwm_value = PWM_TIMER_MAX-1;
		else if (pwm_value < 0)
			pwm_value = 0;



	TIM14->CCR1 = pwm_value;
}
