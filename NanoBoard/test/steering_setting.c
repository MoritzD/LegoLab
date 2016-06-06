/*
 * steering_setting.c
 *
 *  Created on: May 4, 2016
 *      Author: root
 */

#include "steering_setting.h"

void steering_setting(unsigned long phase1, unsigned long duty1,unsigned long phase2, unsigned long duty2,
		unsigned long period,unsigned long enable){

	unsigned long * pwm_en= (unsigned long *)STEERING_PWM_EN;
	unsigned long * pwm_period=(unsigned long *)STEERING_PWM_PERIOD;
	unsigned long * pwm_phase1=(unsigned long *)STEERING_PWM_PHASE1;
	unsigned long * pwm_phase2=(unsigned long *)STEERING_PWM_PHASE2;
	unsigned long * pwm_duty1=(unsigned long *)STEERING_PWM_DUTY1;
	unsigned long * pwm_duty2=(unsigned long *)STEERING_PWM_DUTY2;

	* pwm_en=enable;
	* pwm_period=period;
	* pwm_phase1=phase1;
	* pwm_phase2=phase2;
	* pwm_duty1=duty1;
	* pwm_duty2=duty2;
}

//possible levels are from 0 to 14
//straight is 7
void steering_set_level(char level){
	unsigned long period = 0x186A;
	unsigned long phase1 = 0;
	unsigned long phase2 = 0;
	switch(level){
	case 0:	phase1 = 0x186A;
			phase2 = 0; break;
	case 1:	phase1 = 5357;
			phase2 = 0; break;
	case 2:	phase1 = 4464;
			phase2 = 0; break;
	case 3:	phase1 = 3571;
			phase2 = 0; break;
	case 4:	phase1 = 2679;
			phase2 = 0; break;
	case 5:	phase1 = 1786;
			phase2 = 0; break;
	case 6: phase1 = 893;
			phase2 = 0; break;
	case 7: phase1 = 0;
			phase2 = 0; break;
	case 8: phase2 = 0x186A;
			phase1 = 0; break;
	case 9: phase2 = 5357;
			phase1 = 0; break;
	case 10:phase2 = 4464;
			phase1 = 0; break;
	case 11:phase2 = 3571;
			phase1 = 0; break;
	case 12:phase2 = 2679;
			phase1 = 0; break;
	case 13:phase2 = 1786;
			phase1 = 0; break;
	case 14:phase2 = 893;
			phase1 = 0; break;
	default:phase1 = 0;
			phase2 = 0;
	}
	steering_setting(0, phase1, 0, phase2, period, 3);
}
