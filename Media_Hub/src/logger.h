/*
 * logger.h
 *
 *  Created on: Jul 31, 2014
 *      Author: duyphan
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <syslog.h>

/* define macro for print
 * the currently executing the file of the source code,
 * and the name of the current function.
 * line of source code,
 */
#define LOG_HDR "<<%s::%s::%d>>  "
#define LOG_FTR __FILE__, __FUNCTION__, __LINE__
#define LOG_BUFF_LEN 512
#define appLog(level, format, ...){ \
	writeLog(level, LOG_HDR format, LOG_FTR, ## __VA_ARGS__);\
}



void initLogger();
void closeLogger();

// Interface to be used to log text into syslog file
void writeLog(int logLevel, const char *pLogStr, ...);
#endif /* LOGGER_H_ */
