/*Owen Craig
 * 5/5/25
 *Driver for ADC
 * LAB 5
 */


#include "ES28.h"
#include <ADC.h>

void adc_init(void){ // initializes ADC


	//initializing PA0
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	GPIOA->MODER &= ~GPIO_MODER_MODE0_Msk;
	GPIOA->MODER |= (GPIO_ANALOG <<GPIO_MODER_MODE0_Pos);


	// Configure ADC module
	RCC->APBENR2 |= RCC_APBENR2_ADCEN;	// Enable clock access

	adc_setWidth(12); // set precision (argument is number of bits)
	adc_setChannel(0); // set to channel 0 (for PA0)

	//SETTING REFERENCE VOLTAGE:
	ADC1->CR|=ADC_CR_ADVREGEN; // turn on reference voltage
	//wait startup time
	delay_ms(2);
	// poke calibrate bit
	ADC1->CR|= ADC_CR_ADCAL;
	while(ADC1->CR&ADC_CR_ADCAL);//wait for it to finish

	// Enable ADC module
	ADC1->ISR |= ADC_ISR_ADRDY;			// Clear the ready bit by writing '1' to it
	ADC1->CR |= ADC_CR_ADEN;			// Set the ADC enable bit
}

void adc_setChannel(unsigned int chNum){
	//set channel to number passed in to function
	ADC1->CHSELR = (1U<<chNum);
}

void adc_setWidth( int widthCode){ //sets precision, input is the number of bits of precision
	ADC1->CFGR1 &= ~ADC_CFGR1_RES_Msk; // clear bits
	switch(widthCode){
	case(12):
		ADC1->CFGR1 |= (0U<<ADC_CFGR1_RES_Pos);
	break;
	case(10):
		ADC1->CFGR1 |= (1U<<ADC_CFGR1_RES_Pos);
	break;
	case(8):
		ADC1->CFGR1 |= (2U<<ADC_CFGR1_RES_Pos);
	break;
	case(6):
		ADC1->CFGR1 |= (3U<<ADC_CFGR1_RES_Pos);
	break;
	}
}

int adc_getValue(void){// starts a conversion and returns its result
	while( (ADC1->ISR & ADC_ISR_ADRDY ) == 0){}	// Wait for ADC to be ready
	ADC1->CR |= ADC_CR_ADSTART;					// Start a single conversion

	// Wait for conversion to finish
	while( (ADC1->ISR & ADC_ISR_EOC) == 0 ){}		// Blocking

	// Read converted value
	return (uint16_t) ADC1->DR;

}









