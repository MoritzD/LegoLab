/*
 * stearing_setting.c
 *
 *  Created on: May 4, 2016
 *      Author: root
 */

#include "stearing_setting.h"

void stearing_setting(unsigned long phase1, unsigned long duty1,unsigned long phase2, unsigned long duty2,
		unsigned long period,unsigned long enable){

	unsigned long * pwm_en= (unsigned long *)STEARING_PWM_EN;
	unsigned long * pwm_period=(unsigned long *)STEARING_PWM_PERIOD;
	unsigned long * pwm_phase1=(unsigned long *)STEARING_PWM_PHASE1;
	unsigned long * pwm_phase2=(unsigned long *)STEARING_PWM_PHASE2;
	unsigned long * pwm_duty1=(unsigned long *)STEARING_PWM_DUTY1;
	unsigned long * pwm_duty2=(unsigned long *)STEARING_PWM_DUTY2;

	* pwm_en=enable;
	* pwm_period=period;
	* pwm_phase1=phase1;
	* pwm_phase2=phase2;
	* pwm_duty1=duty1;
	* pwm_duty2=duty2;
}
