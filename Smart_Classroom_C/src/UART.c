/*
 * UART.c
 *
 *  Created on: Dec 17, 2013
 *      Author: d-dragon
 */

#include "UART.h"

void init_UART() {
	//Open dev mode Read/Write, No control terminal, non-blocking mode
	fd_UART = open(DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd_UART < 0) {
		perror(DEVICE);
//		exit(-1);
	}
	//Config UART
	config_UART.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	config_UART.c_iflag = IGNPAR;
	config_UART.c_oflag = 0;
	//set input mode (non-canonical, no echo...
	config_UART.c_lflag = 0;
	config_UART.c_cc[VTIME] = 5; //inter-character timer each 0.5 second
	config_UART.c_cc[VMIN] = 0; // non-blocking read

	tcflush(fd_UART, TCIFLUSH);
	tcsetattr(fd_UART, TCSANOW, &config_UART); //set configuration
}

void read_UART(){
	STOP = FALSE;
	while(STOP == FALSE){
		r_UART = read(fd_UART, buf_UART, sizeof(buf_UART));
		buf_UART[r_UART] = 0;
		if(r_UART > 0){
			STOP = TRUE;
		}
	}
}
int write_UART(char buf_write[]){
	w_UART = write(fd_UART, buf_write, sizeof(buf_write));
	if (w_UART < 0){
		perror("Write Uart:");
	}
	return 0;
}

