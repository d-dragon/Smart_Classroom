/*
 * receiveFile.h
 *
 *  Created on: Jul 11, 2014
 *      Author: d-dragon
 */

#ifndef RECEIVE_FILE_H_
#define RECEIVE_FILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define DEFAULT_PATH "/home/"
#define FILE_ERROR -1
#define FILE_SUCCESS 0

char *path_to_file;
FILE *file_recv;

char *initFileInfra(char *);
FILE *createFileStream(char *str_file_name);
void writetoFileStream();
void *recvFileThread();

#endif /* RECEIVE_FILE_H_ */
