/*
 * logger.c
 *
 *  Created on: Jul 31, 2014
 *      Author: duyphan
 */
#include "logger.h"

void init_logger(){

	openlog("MEDIA_HUB", LOG_PID | LOG_CONS | LOG_NDELAY, LOG_USER);
}

void close_logger(){
	closelog();
}

