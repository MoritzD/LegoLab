/*
 * pin_tests.c
 *
 *  Created on: May 4, 2016
 *      Author: root
 */

#include "pin_tests.h"

//function to test stearing pins (will not terminate!)
//type 1: alternate on pin 1; sets pin 2 to zero
//type 2: alternate on pin 2; sets pin 1 to zero
void test_stearing(unsigned int test_type, unsigned long s_period){

	if(test_type==1){ //test 1
		unsigned long s_duty1 = 0;
		unsigned int i = 0;
		while(1){
			while(s_duty1<s_period){
				steering_setting(0, s_duty1, 0, 0, s_period, 1);
				s_duty1 += 10;
				for(i=0;i<2550;i++); //delay
			}
			while(s_duty1>0){
				steering_setting(0, s_duty1, 0, 0, s_period, 1);
				s_duty1 -= 10;
				for(i=0;i<2550;i++); //delay
			}
		}
	}else if(test_type==2){ //test 2
		unsigned long s_duty2 = 0;
		unsigned int i = 0;
		while(1){
			while(s_duty2<s_period){
				steering_setting(0, 0, 0, s_duty2, s_period, 2);
				s_duty2 += 10;
				for(i=0;i<2550;i++); //delay
			}
			while(s_duty2>0){
				steering_setting(0, 0, 0, s_duty2, s_period, 2);
				s_duty2 -= 10;
				for(i=0;i<2550;i++); //delay
			}
		}
	}
	return;
}

//function to test motor pin (will not terminate)
//test 1: TODO
//test 2: TODO
//test 3: alternate between max value on pin 1 and on pin 2
void test_motor(unsigned int test_type, unsigned long m_period){
	if(test_type==1){ //test 1

	}else if(test_type==2){//test 2

	}else if(test_type==3){//test 3
		unsigned long m_duty1 = 0;
		unsigned long m_duty2 = 0;
		unsigned int i = 0;
		while(1){
			//drive forward
			while(m_duty1<m_period){
				motor_setting(0, m_duty1, 0, m_duty2, m_period, 3);
				m_duty1 += 10;
				for(i=0;i<2550;i++); //delay
			}
			while(m_duty1>0){
				motor_setting(0, m_duty1, 0, m_duty2, m_period, 3);
				m_duty1 -= 10;
				for(i=0;i<2550;i++); //delay
			}

			//drive backwards
			m_duty1 = 0;
			while(m_duty2<m_period){
				motor_setting(0, m_duty1, 0, m_duty2, m_period, 3);
				m_duty2 += 10;
				for(i=0;i<2550;i++); //delay
			}
			while(m_duty2>0){
				motor_setting(0, m_duty1, 0, m_duty2, m_period, 3);
				m_duty2 -= 10;
				for(i=0;i<2550;i++); //delay
			}
			m_duty2 = 0;
		}
	}
	return;
}

