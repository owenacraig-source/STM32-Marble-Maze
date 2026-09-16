/*
 * Owen Craig
 * ADC driver header file
 * 5/5/25
 * Lab 5
 */

#ifndef ADC_H_
#define ADC_H_

void adc_init(void); // initializes ADC
void adc_setChannel(unsigned int chNum); //set channel to number passed in to function
void adc_setWidth( int widthCode); //sets precision, input is the number of bits of precision
int adc_getValue(void);// starts a conversion and returns its result


#endif /*ADC_H_*/
