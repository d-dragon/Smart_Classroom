/*
 * receiveFile.h
 *
 *  Created on: Jul 11, 2014
 *      Author: d-dragon
 */

#ifndef RECEIVE_FILE_H_
#define RECEIVE_FILE_H_



#define DEFAULT_PATH "/home/"
#define FILE_PATH_LEN_MAX 200
#define FILE_ERROR -1
#define FILE_SUCCESS 0

FILE *g_file_stream;
int g_file_size;
pthread_t g_File_Handler_Thd;

extern pthread_mutex_t g_file_buff_mutex;

typedef struct file{
	int index;
	char *filename;
}FileInfo;


char *initFileInfra(char *);
FILE *createFileStream(char *);
void writetoFileStream();
void *recvFileThread();
void *FileStreamHandlerThread(char *);
void closeFileStream();


#endif /* RECEIVE_FILE_H_ */
