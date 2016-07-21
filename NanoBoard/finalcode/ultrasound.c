/*
 * ultrasound.c
 *
 *  Created on: 2016��5��6��
 *      Author: Laowang
 */

#include "uart.h"
#include "ultrasound.h"
#include <stdio.h>


/****************************************************************************
  Function:
    void ultrasound_init(long Address)

  Description:
    This function initializes the UART for the Ultrasound.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void ultrasound_init(long Address)
{
	uart_init(Address, 0x00);
}



/****************************************************************************
  Function:
    unsigned int ultrasound_read(long Address, unsigned char settings)

  Description:
   // This function receives one Byte via the corresponding UART from the Address

  Precondition:
    Call ultrasound_init(long Address) prior to use this function

  Parameters:
    long Address 			- Address of the UART (and therefore of the Ultrasound)
	unsigned char settings 	- defines maximum distance to measure (1: 11m; 0: 5m)

  Returns:
    unsigned int - distance in mm

  Remarks:
    Distance in meter
  ***************************************************************************/
unsigned int ultrasound_read(long Address)
{
	unsigned int i;
	unsigned char res_l, res_h;
	uart_send_byte(Address, 0xe8);

	for (i = 0; i < 2000; i++); //delay

	uart_send_byte(Address, 0x02);

	for (i = 0; i < 2000; i++); //delay

	uart_send_byte(Address, 0x0a); //0x0a -> range (0m-1m)

	res_h = uart_receive_byte(Address);

	//for (i = 0; i < 1000; i++); //delay

	res_l = uart_receive_byte(Address);

	//printf("res_h is %x, res_l is %x.\n", res_h, res_l);

	return (((unsigned int) res_h) << 8) + res_l;
}

