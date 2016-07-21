/*
 * raspberr_uart.cpp
 *
 *  Created on: Jun 6, 2016
 *      Author: root
 */

#include "uart.h"
#include "raspberry_uart.h"

void raspberry_init(long Address){
	uart_init(Address, 0x0);
}

unsigned char raspberry_read(long Address){
	return uart_receive_byte(Address);
}

unsigned char raspberry_test(long Address){
	uart_send_byte(Address, '1');

	return uart_receive_byte(Address);
}

