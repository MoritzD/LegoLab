/*
 * pin_tests.h
 *
 *  Created on: May 4, 2016
 *      Author: root
 */

#ifndef PIN_TESTS_H_
#define PIN_TESTS_H_

#include "motor_setting.h"
#include "steering_setting.h"

void test_stearing(unsigned int test_type, unsigned long s_period);
void test_motor(unsigned int test_type, unsigned long m_period);

#endif /* PIN_TESTS_H_ */
