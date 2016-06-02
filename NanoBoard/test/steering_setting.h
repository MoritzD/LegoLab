/*
 * steering_setting.h
 *
 *  Created on: May 4, 2016
 *      Author: root
 */

#ifndef STEERING_SETTING_H_
#define STEERING_SETTING_H_

#include "system.h"

#define STEERING_PWM_EN PWM1_BASE
#define STEERING_PWM_PERIOD PWM1_BASE+1
#define STEERING_PWM_DUTY1 PWM1_BASE+2
#define STEERING_PWM_DUTY2 PWM1_BASE+3
#define STEERING_PWM_PHASE1 PWM1_BASE+4
#define STEERING_PWM_PHASE2 PWM1_BASE+5

void steering_setting(unsigned long phase1, unsigned long duty1,unsigned long phase2, unsigned long duty2,
		unsigned long period,unsigned long enable);

void steering_set_level(char);


#endif /* STEARING_SETTING_H_ */
