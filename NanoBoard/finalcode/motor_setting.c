/*
 * motor_setting.c
 *
 *  Created on: 07.11.2012
 *      Author: karl_ee
 *          input parameters:
 *      	phase: range from 0 to value of period
 *     		duty cycle: set to number=period*percentage of duty cycle
 *      	period: according to the freq of cpu,
 *      			normally the period should be set to the value
 *      			that makes the freq of pwm waveform to be 15 k
 *      			e.g., for 50MHz, value should be 3333(0xD05)
 *      	enable:'0' represents off, '1' is on, lease significant bit is for channel 1
 *      			e.g., for channel 1 on and channel 2 off, enable=0x1
 *
 */

#include "motor_setting.h"

//phase1 -> forward
//phase2 -> backward
//will only set one pin greater zero
void motor_setting(unsigned long phase1, unsigned long duty1,unsigned long phase2, unsigned long duty2,
		unsigned long period,unsigned long enable)
{
	if(phase1>0 && phase2>0){
		if(phase1>=phase2) phase2 = 0;
		else phase1 = 0;

	}

	unsigned long * pwm_en= (unsigned long *)MOTOR_PWM_EN;
	unsigned long * pwm_period=(unsigned long *)MOTOR_PWM_PERIOD;
	unsigned long * pwm_phase1=(unsigned long *)MOTOR_PWM_PHASE1;
	unsigned long * pwm_phase2=(unsigned long *)MOTOR_PWM_PHASE2;
	unsigned long * pwm_duty1=(unsigned long *)MOTOR_PWM_DUTY1;
	unsigned long * pwm_duty2=(unsigned long *)MOTOR_PWM_DUTY2;

	* pwm_en=enable;
	* pwm_period=period;
	* pwm_phase1=phase1;
	* pwm_phase2=phase2;
	* pwm_duty1=duty1;
	* pwm_duty2=duty2;

}
