/*
 * motor_setting.h
 *
 *  Created on: 07.11.2012
 *      Author: karl_ee
 *          input parameters:
 *      	phase: range from
 *     		duty cycle:
 *      	period: according to the freq of cpu,
 *      			normally the period should be set to the value
 *      			that makes the freq of pwm waveform to be 15 k
 *      			e.x for 50MHz, value should be 3333(0xD05)
 *      	enable:'0' represents off, '1' is on
 */

#ifndef MOTOR_SETTING_H_
#define MOTOR_SETTING_H_

#include "system.h"

#define MOTOR_PWM_EN PWM0_BASE
#define MOTOR_PWM_PERIOD PWM0_BASE+1
#define MOTOR_PWM_DUTY1 PWM0_BASE+2
#define MOTOR_PWM_DUTY2 PWM0_BASE+3
#define MOTOR_PWM_PHASE1 PWM0_BASE+4
#define MOTOR_PWM_PHASE2 PWM0_BASE+5

void motor_setting(unsigned long phase1, unsigned long duty1,unsigned long phase2, unsigned long duty2,
		unsigned long period,unsigned long enable);
#endif /* MOTOR_SETTING_H_ */
