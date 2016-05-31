/*
 * uart.cpp
 *
 *  Created on: 01.06.2016
 *      Author: Berkay
 */

#include "uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int main() {

int uart0_filestream = -1;
uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY); //Initialisierung der UART
if (uart0_filestream == -1) {
	printf("[ERROR] UART open()\n");
}

struct termios options;
tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
tcflush(uart0_filestream, TCIFLUSH);
tcsetattr(uart0_filestream, TCSANOW, &options);


// sendung bytes über tx-pin
unsigned char BUF_TX[5];
unsigned char *TX;

TX = &BUF_TX[0];
*TX++ = 'HELLO';
*TX++ = 'Probe!!!!!!';


if (uart0_filestream != -1)	{
	int out = write(uart0_filestream, &BUF_TX[0], (TX - &BUF_TX[0])); //macht das Senden, out zaehlt die geschriebene bytes
	if (out < 0) {
		printf("[ERROR] UART TX\n");
	} else {
		printf("[STATUS: TX %i Bytes] %s\n", out, BUF_TX);
	}
} // if uart0

sleep(1);


// Bytes empfangen
if (uart0_filestream != -1) {
	unsigned char BUF_RX[50];
	int rx_length = read(uart0_filestream, (void*)BUF_RX, 50); //rx_length zaehlt ankommenden bytes
//UBERPRÜFUNG, 0'dan kucukse bir hata var, 0'dan buyukse daha fayla bytes gelemiyor
	if (rx_length < 0) {
		printf("[ERROR] UART RX\n");
	} else if (rx_length == 0) {
		printf("[ERROR] UART RX - no data\n");
	} else {
		BUF_RX[rx_length] = '\0';
		printf("[STATUS: RX %i Bytes] %s\n", rx_length, BUF_RX);
	} //rx_length check
} //if uart0

close(uart0_filestream);

return 0;
}
