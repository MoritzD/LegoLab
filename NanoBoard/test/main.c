/*
 * main.c
 *
 *  Created on: 07.11.2012
 *      Author: karl_ee
 */

#include "main.h"

int main()
{
	printf("Running!\n");




	return 0;
}

void drive_by_sensors(){

	printf("Drive Car via Ultrasound Sensors.");

	//Assign motor pins
	unsigned long m_phase1=0x0,m_phase2=0x000,m_duty1=0x0,m_duty2=0x0,m_period=0x186A,m_enable=0x3;
	motor_setting(m_phase1,m_duty1,m_phase2,m_duty2,m_period,m_enable);

	//Assign steering pins
	unsigned long s_phase1=0x0,s_phase2=0x000,s_duty1=0x0,s_duty2=0x0,s_period=0x186A,s_enable=0x3;
	steering_setting(s_phase1,s_duty1,s_phase2,s_duty2,s_period,s_enable);

	//Intit Ultrasound
	unsigned int distance0, distance1, distance2, i, j = 0;
	ultrasound_init(NEW_ULTRASOUND0_BASE);
	ultrasound_init(NEW_ULTRASOUND1_BASE);
	ultrasound_init(NEW_ULTRASOUND2_BASE);

	//Print sensor values
	/*while(1){
		distance0 = ultrasound_read(NEW_ULTRASOUND0_BASE) * 170;
		distance1 = ultrasound_read(NEW_ULTRASOUND1_BASE) * 170;
		distance2 = ultrasound_read(NEW_ULTRASOUND2_BASE) * 170; //distance values in um
		printf("Distance0: %ium Distance1: %ium Distance2: %ium\n", distance0, distance1, distance2);

		for(i = 0; i<2550; i++){
			for(j = 0; j<30; j++);
		} //got delay value through testing

	}*/

	while(1){
		//Get sensor values
		distance0 = ultrasound_read(NEW_ULTRASOUND0_BASE) * 170;
		distance1 = ultrasound_read(NEW_ULTRASOUND1_BASE);
		distance2 = ultrasound_read(NEW_ULTRASOUND2_BASE);


		//Check for possible collision
		if(distance0>50000){
			m_duty1 = 0x186A;
		}else{
			m_duty1 = 0x0;
		}


		if(distance1>893){ //893 is about 0,15181m
			s_duty1 = 0x0;
		}else{
			//7 and 1000 are constants calculated with a period of 0x186A
			//Add 1000 to have max value at 2,5cm (will result in duty > 100%)
			s_duty1 = s_period - distance1*7 + 1000;
		}
		if(distance2>893){
			s_duty2 = 0x0;
		}else{
			s_duty2 = s_period - distance2*7 + 1000;
		}

		//Set PWM-Signals
		steering_setting(s_phase1,s_duty1,s_phase2,s_duty2,s_period,s_enable);
		motor_setting(m_phase1, m_duty1, m_phase2, m_duty2, m_period, m_enable);

		float duty1pr = ((float) s_duty1) / 6251.0;
		float duty2pr = ((float) s_duty2) / 6251.0;
		printf("Steering duty1 is: %f\twith distance: %i\n", duty1pr, distance1);
		printf("Steering duty2 is: %f\twith distance: %i\n", duty2pr, distance2);
		for(i = 0; i<2550; i++){
			for(j = 0; j<10; j++);
		} //Got delay value through testing; prevents sensors from crashing

	}

}

void drive_by_rasp_input(){

	printf("Drive Car via Raspberry Pi input.");

	//Assign motor pins
	unsigned long m_phase1=0x0,m_phase2=0x000,m_duty1=0x0,m_duty2=0x0,m_period=0x186A,m_enable=0x3;
	motor_setting(m_phase1,m_duty1,m_phase2,m_duty2,m_period,m_enable);

	//Intit Ultrasound
	unsigned int distance, i, j = 0;
	ultrasound_init(NEW_ULTRASOUND0_BASE);

	//Init Uart to rasp pi
	raspberry_init(UART_0_BASE);

	while(1){

		distance = ultrasound_read(NEW_ULTRASOUND0_BASE);

		//Check for collision
		if(distance > 300){
			m_duty1=0x186A;
		}else{
			m_duty1=0;
		}

		//Set steering PWM signal
		steering_set_level(raspberry_read(UART_0_BASE));

		//Set motor PWM signal
		motor_setting(m_phase1,m_duty1,m_phase2,m_duty2,m_period,m_enable);

		for(i = 0; i<2550; i++){
			for(j = 0; j<10; j++);
		} //Got delay value through testing; prevents sensors from crashing
	}
}






