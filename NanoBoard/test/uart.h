/*
 * uart.h
 *
 *  Created on: 2016��5��6��
 *      Author: Laowang
 */

#ifndef UART_H_
#define UART_H_

void uart_init(long, unsigned char);
void uart_send_byte(long, unsigned char);
unsigned char uart_receive_byte(long);

#endif /* UART_H_ */
