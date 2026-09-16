/*
 * Owen Craig
 * ADC driver header file
 * 5/5/25
 * Lab 5
 */

#ifndef TB6612_H_
#define TB6612_H_

void motor_init(void); // initialize pins and PWM timer
void motor_mode(uint8_t ); // sets mode of motor
void motor_speed(uint16_t ); // sets motor speed with PWM counter value

#endif /*TB6612_H_*/
