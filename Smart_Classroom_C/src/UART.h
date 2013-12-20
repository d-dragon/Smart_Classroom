/*
 * UART.h
 *
 *  Created on: Dec 17, 2013
 *      Author: d-dragon
 */

#ifndef UART_H_
#define UART_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>

#define BAUDRATE B9600
#define DEVICE "/dev/ttyO4"
#define FALSE 0
#define TRUE 1

volatile int STOP;
int fd_UART, r_UART, w_UART;
struct termios config_UART;
char buf_UART[10];

void init_UART();
void read_UART();
int write_UART(char[]);


#endif /* UART_H_ */
