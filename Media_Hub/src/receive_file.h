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
#include <pthread.h>

#define DEFAULT_PATH "/home/"
#define FILE_ERROR -1
#define FILE_SUCCESS 0

char *g_path_to_file;
FILE *g_file_stream;
int g_file_size;
pthread_t g_File_Handler_Thd;

extern pthread_mutex_t g_file_buff_mutex;


char *initFileInfra(char *);
FILE *createFileStream(char *);
void writetoFileStream();
void *recvFileThread();
void *FileStreamHandlerThread(char *);


#endif /* RECEIVE_FILE_H_ */
