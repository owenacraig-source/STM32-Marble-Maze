/*
 * Owen Craig
 * 
 *
 * 5/29/25
 *
 * ENGS 28 final project
 * marble maze with joystick control
 *
 *
 */


#include <stdio.h>
#include "ES28.h"
#include "uart.h"
#include "SevenSeg.h"
#include "i2c.h"
#include "adc.h"


void joystick_init(void);
void pb4_exti_init(void);
void tim14_interrupt_init(void);
void LED_pins_init(void);
void pa5_exti_init(void);
void pa6_exti_init(void);
void tim16_ms_interrupt_init(int );

volatile int LEDblink = 0;
volatile int buttonPush = 0;
volatile int startFlag = 0;
volatile int endFlag = 0;
volatile int timerFlag = 0;


int main(void){

	uint8_t mode = 0;
	uint16_t stopwatch;
	uint8_t min,sec;
	uint8_t displayBuffer[10];
	uint16_t Ypos, Xpos, Xsteps, Ysteps, i;

	uart2_init();
	printf("uart initialized\n\r");


	pb4_exti_init();
	tim14_interrupt_init();
	LED_pins_init();

	pa5_exti_init();
	pa6_exti_init();
	tim16_ms_interrupt_init(1000);

	i2c1_init();
	SevenSeg_init();
	adc_init();
	joystick_init();


	while(1){

		//printf("%d\n\r", mode);
		// if in off mode, red led on
		if (mode == 0) {
			GPIOB->ODR &= ~GPIO_ODR_OD10;
			GPIOB->ODR |= GPIO_ODR_OD3;
		}else {
			//IF ON




			//SET GREEN LED
			GPIOB->ODR &= ~GPIO_ODR_OD3;//if game is on led is off
			if(LEDblink){
				LEDblink = 0; //blink green led if game is on
				GPIOB->ODR ^=GPIO_ODR_OD10;
			}

			//ANTICIPATE INTERRUPT FROM PHOTOSENSORS

			if(startFlag){ //start interrupt is a falling edge
				startFlag = 0;


				/* Enable timer */
				TIM16->CR1 |= TIM_CR1_CEN;
				stopwatch = 0;
				printf("START!\n\r");
				//SevenSeg_number(0, displayBuffer);
				//SevenSeg_write(displayBuffer);
			}

			if(endFlag){
				endFlag = 0;
				//disable timer
				TIM16->CR1 &= ~TIM_CR1_CEN;
				printf("END!\n\r");
				SevenSeg_blink(1);
				delay_ms(2000);
				SevenSeg_blink(0);

			}
			//increment stopwatch on seven seg every second
			if(timerFlag){
				timerFlag = 0;
				stopwatch++;
				min = stopwatch/60;
				sec = stopwatch%60;
				printf("%0d:%0d\n\r", min, sec);

				//displays minutes and seconds to seven Seg
				displayBuffer[0] = numbertable[min/10];
				displayBuffer[2] = numbertable[min%10];
				displayBuffer[4] = 0b11111111;
				displayBuffer[6] = numbertable[sec/10];
				displayBuffer[8] = numbertable[sec%10];

				SevenSeg_write(displayBuffer);

				//2 minute time limit
				if(stopwatch==60) {
					mode = 0;
					displayBuffer[0] = 0b01110001;
					displayBuffer[2] = 0b01110111;
					displayBuffer[4] = 0;
					displayBuffer[6] = 0b00110000;
					displayBuffer[8] = 0b00111000;
					SevenSeg_write(displayBuffer);
				}





			}


			if(milliseconds()%50==0){//sample every 50 milliseconds

				//POLL THE JOYSTICK AND MOVE MOTORS


				//joystick Y
				adc_setChannel(0);
				Ypos = adc_getValue();
				//joystick X
				adc_setChannel(1);
				Xpos = adc_getValue();
				printf("X: %d Y: %d\n\r", Xpos, Ypos);




				/*
				 *  Y POSITION
				 */


				if(Ypos<2000){
					//AWAY
					GPIOB->ODR|= GPIO_ODR_OD0; //set PB0 for DIRECTION OF Y
					Ysteps = (4096-Ypos)/1000;
				}else if (Ypos>2096){
					//TOWARDS
					GPIOB->ODR &= ~GPIO_ODR_OD0; //clear PB0
					Ysteps = (Ypos-2048)/1000;

				}else{
					//deadband
					Ysteps = 0;
				}

				//stepping loop: PA7 for STEPS OF Y
				for(i=0;i<Ysteps;i++){
					GPIOA->ODR|= GPIO_ODR_OD7;
					delay_ms(.5);
					GPIOA->ODR &= ~GPIO_ODR_OD7;
					delay_ms(.5);
				}



				/*
				 * X POSITION
				 */
				if(Xpos>2096){
					//right
					GPIOA->ODR|= GPIO_ODR_OD9; //set PA9 for DIRECTION OF X
					Xsteps = (Xpos-2048)/1000;
				}else if (Xpos<2000){
					//left
					GPIOA->ODR &= ~GPIO_ODR_OD9; //clear PA9
					Xsteps = (4096-Xpos)/1000;
				}else{
					//deadband
					Xsteps = 0;
				}

				//stepping loop: PC7 for STEPS OF X
				for(i=0;i<Xsteps;i++){
					GPIOC->ODR|= GPIO_ODR_OD7;
					delay_ms(.5);
					GPIOC->ODR &= ~GPIO_ODR_OD7;
					delay_ms(.5);
				}
			}





		}

		if(buttonPush){
			buttonPush = 0;
			// if button is pushed, switch mode
			if(mode==1){
				mode = 0;
			}else{
				mode = 1;
				//clear start and stop flags that may have been triggered while in off mode
				startFlag = 0;
				endFlag = 0;
			}



		}


	}
	return(0);
}







// Interrupt service routine for the User button
void EXTI4_15_IRQHandler(void){
	// disable global interrupt
	__disable_irq();

	// Check that EXTI5 interrupted
	if (EXTI->FPR1 & EXTI_FPR1_FPIF4) {
		EXTI->FPR1 = EXTI_FPR1_FPIF4;	// Clear PR flag by writing 1 into it
		buttonPush = 1;
	}


	// Check that EXTI5 interrupted
	if (EXTI->FPR1 & EXTI_FPR1_FPIF5) {
		EXTI->FPR1 = EXTI_FPR1_FPIF5;	// Clear PR flag by writing 1 into it
		startFlag = 1;
	}

	if (EXTI->RPR1 & EXTI_RPR1_RPIF6) {
		EXTI->RPR1 = EXTI_RPR1_RPIF6;	// Clear PR flag by writing 1 into it
		endFlag = 1;
	}

	// Enable global interrupts
	__enable_irq();
}


void TIM14_IRQHandler(void) {
	__disable_irq();
	/* Clear UIF in status register */
	TIM14->SR &= ~TIM_SR_UIF;
	LEDblink = 1;
	__enable_irq();
}





//falling edge interrupt on PB4

void pb4_exti_init(void) {
	// disable global interrupt
	__disable_irq();

	// Enable clock access to GPIOB
	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;

	// Configure PB4 as input pin (not necessary since this is default reset value)
	GPIOB->MODER &= ~GPIO_MODER_MODE4_Msk;
	GPIOB->MODER |= (GPIO_INPUT << GPIO_MODER_MODE4_Pos);

	// enable pull-up for PB4
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD4_Msk;
	GPIOB->PUPDR |= (GPIO_PULLUP << GPIO_PUPDR_PUPD4_Pos);

	// Configure EXTICR2
	// Note: EXTICR1..4 are accessed as EXTICR[0..3]
	EXTI->EXTICR[1] &= ~EXTI_EXTICR2_EXTI4_Msk;
	EXTI->EXTICR[1] |= (EXTI_PB << EXTI_EXTICR2_EXTI4_Pos);

	// Select falling edge trigger for line EXTI4
	EXTI->FTSR1 |= EXTI_FTSR1_FT4;
	EXTI->RTSR1 &= ~EXTI_RTSR1_RT4;

	// Unmask line EXTI4
	EXTI->IMR1 |= EXTI_IMR1_IM4;

	// Enable EXTI4 line in NVIC
	NVIC_EnableIRQ(EXTI4_15_IRQn);

	// Enable global interrupts
	__enable_irq();
}


void tim14_interrupt_init() {
	// disable global interrupt
	__disable_irq();

	// enable clock access to timer 14 (on APB bus)
	RCC->APBENR2 |= RCC_APBENR2_TIM14EN;

	// Set prescaler value
	TIM14->PSC = 12000 - 1; // starts counting at 0

	// Set auto-reload value
	TIM14->ARR = 1000 - 1;

	// Clear counter
	TIM14->CNT = 0;

	// Enable update interrupt
	TIM14->DIER |= TIM_DIER_UIE;

	// Enable timer interrupt in NVIC
	NVIC_EnableIRQ(TIM14_IRQn);

	// Enable timer
	TIM14->CR1 |= TIM_CR1_CEN;

	// Enable global interrupts
	__enable_irq();
}


void LED_pins_init(void){
	//set pb3 and pb10 as output pins
	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;

	//RED LED on PB3
	GPIOB->MODER &= ~GPIO_MODER_MODE3_Msk;
	GPIOB->MODER |= (GPIO_OUTPUT<<GPIO_MODER_MODE3_Pos);

	//GREEN LED on PB10
	GPIOB->MODER &= ~GPIO_MODER_MODE10_Msk;
	GPIOB->MODER |= (GPIO_OUTPUT<<GPIO_MODER_MODE10_Pos);

}



void TIM16_IRQHandler(void) {
	__disable_irq();
	/* Clear UIF in status register */
	TIM16->SR &= ~TIM_SR_UIF;
	timerFlag = 1;
	__enable_irq();
}




// start of maze - falling edge on PA5
// GREEN YELLOW BLUE WIRE SENSOR
void pa5_exti_init(void) {
	// disable global interrupt
	__disable_irq();

	// Enable clock access to GPIOA
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	// Configure PA5 as input pin (not necessary since this is default reset value)
	GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk;
	GPIOA->MODER |= (GPIO_INPUT << GPIO_MODER_MODE5_Pos);

	// enable pull-up for PA5
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5_Msk;
	GPIOA->PUPDR |= (GPIO_PULLUP << GPIO_PUPDR_PUPD5_Pos);

	// Configure EXTICR2
	// Note: EXTICR1..4 are accessed as EXTICR[0..3]
	EXTI->EXTICR[1] &= ~EXTI_EXTICR2_EXTI5_Msk;
	EXTI->EXTICR[1] |= (EXTI_PA << EXTI_EXTICR2_EXTI5_Pos);

	// Select falling edge trigger for line EXTI5
	EXTI->FTSR1 |= EXTI_FTSR1_FT5;
	EXTI->RTSR1 &= ~EXTI_RTSR1_RT5;

	// Unmask line EXTI5
	EXTI->IMR1 |= EXTI_IMR1_IM5;

	// Enable EXTI5 line in NVIC
	NVIC_EnableIRQ(EXTI4_15_IRQn);

	// Enable global interrupts
	__enable_irq();
}


// end of maze - rising edge on PA6
// RED BLACK BROWN WIRE SENSOR
void pa6_exti_init(void) {
	// disable global interrupt
	__disable_irq();

	// Enable clock access to GPIOA
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	// Configure PA6 as input pin (not necessary since this is default reset value)
	GPIOA->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOA->MODER |= (GPIO_INPUT << GPIO_MODER_MODE6_Pos);

	// enable pull-up for PA6
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD6_Msk;
	GPIOA->PUPDR |= (GPIO_PULLUP << GPIO_PUPDR_PUPD6_Pos);

	// Configure EXTICR2
	// Note: EXTICR1..4 are accessed as EXTICR[0..3]
	EXTI->EXTICR[1] &= ~EXTI_EXTICR2_EXTI6_Msk;
	EXTI->EXTICR[1] |= (EXTI_PA << EXTI_EXTICR2_EXTI6_Pos);

	// Select rising edge trigger for line EXTI5
	EXTI->RTSR1 |= EXTI_RTSR1_RT6;
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT6;


	// Unmask line EXTI6
	EXTI->IMR1 |= EXTI_IMR1_IM6;

	// Enable EXTI6 line in NVIC
	NVIC_EnableIRQ(EXTI4_15_IRQn);

	// Enable global interrupts
	__enable_irq();
}


void tim16_ms_interrupt_init(int milliseconds) {
	/* enable clock access to timer 16 (on APB bus) */
	RCC->APBENR2 |= RCC_APBENR2_TIM16EN;

	/* Set prescaler value */
	TIM16->PSC = 12000-1; // starts counting at 0

	/* Set auto-reload value */
	TIM16->ARR = milliseconds - 1;

	/* Clear counter */
	TIM16->CNT = 0;

	/* Enable update interrupt */
	TIM16->DIER |= TIM_DIER_UIE;

	/* Enable timer interrupt in NVIC */
	NVIC_EnableIRQ(TIM16_IRQn);

	/* NOTE: timer is enabled in the main loop for precision*/
}

void joystick_init(void){
	//set PA0 and PA1 as analog pins
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;


	GPIOA->MODER &= ~GPIO_MODER_MODE0_Msk;
	GPIOA->MODER |= (GPIO_ANALOG<<GPIO_MODER_MODE0_Pos);

	GPIOA->MODER &= ~GPIO_MODER_MODE1_Msk;
	GPIOA->MODER |= (GPIO_ANALOG<<GPIO_MODER_MODE1_Pos);

	//PA7 PB0 PC7 PA9 as output pins

	//PA7
	GPIOA->MODER &= ~GPIO_MODER_MODE7_Msk;
	GPIOA->MODER |= (GPIO_OUTPUT<<GPIO_MODER_MODE7_Pos);

	//PA9
	GPIOA->MODER &= ~GPIO_MODER_MODE9_Msk;
	GPIOA->MODER |= (GPIO_OUTPUT<<GPIO_MODER_MODE9_Pos);

	//PC7
	GPIOC->MODER &= ~GPIO_MODER_MODE7_Msk;
	GPIOC->MODER |= (GPIO_OUTPUT<<GPIO_MODER_MODE7_Pos);

	//PB0
	GPIOB->MODER &= ~GPIO_MODER_MODE0_Msk;
	GPIOB->MODER |= (GPIO_OUTPUT<<GPIO_MODER_MODE0_Pos);

}


