/*
 * GPIO.h
 *
 *  Created on: Nov 13, 2013
 *      Author: d-dragon
 */

#ifndef GPIO_H_
#define GPIO_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

/**********************************************
 * define									  *
 **********************************************/
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT 3000
#define MAX_BUF 64

typedef enum {
	INPUT_PIN = 0,
	OUTPUT_PIN= 1
} PIN_DIRECTION;
typedef enum {
	ON = 0,
	OFFF = 1
} PIN_VALUE;

/**********************************************
 * Declare Function							  *
 **********************************************/

int gpio_export(unsigned int);
int gpio_unexport(unsigned int);
int gpio_set_dir(unsigned int, PIN_DIRECTION);
int gpio_set_value(unsigned int, PIN_VALUE);
int gpio_get_value(unsigned int,unsigned int*);
int gpio_set_edge(unsigned int, char*);
int gpio_fd_open(unsigned int);
int gpio_fd_close(int);


#endif /* GPIO_H_ */
