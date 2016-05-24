/*
 * stearing_setting.h
 *
 *  Created on: May 4, 2016
 *      Author: root
 */

#ifndef STEARING_SETTING_H_
#define STEARING_SETTING_H_

#include "system.h"

#define STEARING_PWM_EN PWM1_BASE
#define STEARING_PWM_PERIOD PWM1_BASE+1
#define STEARING_PWM_DUTY1 PWM1_BASE+2
#define STEARING_PWM_DUTY2 PWM1_BASE+3
#define STEARING_PWM_PHASE1 PWM1_BASE+4
#define STEARING_PWM_PHASE2 PWM1_BASE+5

void stearing_setting(unsigned long phase1, unsigned long duty1,unsigned long phase2, unsigned long duty2,
		unsigned long period,unsigned long enable);



#endif /* STEARING_SETTING_H_ */
