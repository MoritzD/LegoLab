/*
 * uart.c
 *
 *  Created on: 2016��5��6��
 *      Author: Laowang
 */

#include "altera_avalon_uart.h"
#include "altera_avalon_uart_regs.h"
#include "uart.h"
#include "stdio.h"

/****************************************************************************
  Function:
    void uart_init(long Address, unsigned char flag)

  Description:
    This function initializes the UART

  Precondition:
    None

  Parameters:
    long Address 		- UART Address
	unsigned char flag	- Control-Flag

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void uart_init(long Address, unsigned char flag)
{
	IOWR_ALTERA_AVALON_UART_CONTROL(Address, flag);
    IOWR_ALTERA_AVALON_UART_STATUS(Address, 0x00);
}

/****************************************************************************
  Function:
    void uart_send_byte(long Address, unsigned char data)

  Description:
    This function sends one Byte via the corresponding UART from the Address

  Precondition:
    Call uart_init(long Address, unsigned char flag) prior to use this function

  Parameters:
    long Address 		- UART Address
	unsigned char data	- Data-Byte

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void uart_send_byte(long Address, unsigned char data)
{
	unsigned int status;
    IOWR_ALTERA_AVALON_UART_TXDATA(Address,data);
    status=IORD_ALTERA_AVALON_UART_STATUS(Address);
    while(!(status&0x0040))
    {
    	//printf("Status in send is: %i\n", status);
    	status=IORD_ALTERA_AVALON_UART_STATUS(Address);
    }
}

/****************************************************************************
  Function:
    unsigned char uart_receive_byte(long Address)

  Description:
    This function receives one Byte via the corresponding UART from the Address

  Precondition:
    Call uart_init(long Address, unsigned char flag) prior to use this function

  Parameters:
    long Address - UART Address

  Returns:
    unsigned char res - received Byte

  Remarks:
    None
  ***************************************************************************/
unsigned char uart_receive_byte(long Address)
{
	unsigned int  status;
	unsigned char res;
    status=IORD_ALTERA_AVALON_UART_STATUS(Address);
    while(!(status&0x0080))
    {
    	//printf("Status in receive is: %i\n", status);
    	status=IORD_ALTERA_AVALON_UART_STATUS(Address);
    }
    res=IORD_ALTERA_AVALON_UART_RXDATA(Address);
    return res;
}



