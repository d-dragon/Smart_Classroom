/*
 * receive_file.c
 *
 *  Created on: Jul 12, 2014
 *      Author: d-dragon
 */

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>

#include "receive_file.h"
#include "sock_infra.h"
#include "logger.h"
#include "acpHandler.h"


FILE *createFileStream(char *FileName) {

	FILE *tmp_file;
	char *path_to_file;
	path_to_file = calloc(FILE_PATH_LEN_MAX, sizeof(char));

	appLog(LOG_INFO, "FileName: %s\n", FileName);
	sprintf(path_to_file, "%s%s", DEFAULT_PATH, FileName);
	appLog(LOG_INFO, "path to file: %s\n", path_to_file);

	tmp_file = fopen(path_to_file, "w");
	if (tmp_file == NULL) {
		appLog(LOG_ERR, "fopen failed\n");
		return NULL;
	}
	appLog(LOG_DEBUG, "File stream was created successfully\n");

	return tmp_file;
}



void *FileStreamHandlerThread() {

	int szwrite = 0;

	appLog(LOG_DEBUG, "inside FileStreamHandlerThread--------\n");
	while (!g_StartTransferFlag && g_RecvFileFlag == RECV_FILE_DISABLED) {
		g_waitCount++;
		if (g_waitCount <= MAX_WAITING_COUNT) {
			usleep(1000);
		} else {
			appLog(LOG_DEBUG,
					"Wating start transfer file timeout---thread exit\n");
			g_RecvFileFlag = RECV_FILE_DISABLED;
			pthread_exit(NULL);

		}
	}
	wrapperControlResp(CTRL_RESP_ALREADY);
	appLog(LOG_DEBUG, "Start Transfer file-------------\n");
	pthread_mutex_init(&g_file_buff_mutex, NULL);
	g_RecvFileFlag = RECV_FILE_ENABLED;
	while (1) {
		while (g_writeDataFlag != ENABLED) {

		}
		pthread_mutex_lock(&g_file_buff_mutex);
		//check transfer file flow finished--thread exit
		if(g_RecvFileFlag == RECV_FILE_DISABLED){
			g_writeDataFlag = DISABLED;
			closeFileStream();
			appLog(LOG_DEBUG, "Transfer file completed, FileStreamHandlerThread exited");

			pthread_mutex_unlock(&g_file_buff_mutex);
			pthread_mutex_destroy(&g_file_buff_mutex);

			pthread_exit(NULL);
		}
		//write data to file
		szwrite = fwrite(g_FileBuff, 1, num_byte_read, g_file_stream);
		//				printf("%d bytes was written\n", szwrite);
		if (szwrite < num_byte_read) {
			appLog(LOG_ERR, "write data to stream file failed!\n");
		} else if (szwrite == num_byte_read) {
			appLog(LOG_DEBUG, "wrote %d byte to file stream\n",
					(int) num_byte_read);
			g_writeDataFlag = DISABLED;
			pthread_mutex_unlock(&g_file_buff_mutex);

		}
		/*		pthread_mutex_lock(g_file_buff_mutex);
		 pthread_cond_wait(g_file_thread_cond, g_file_buff_mutex);
		 pthread_mutex_unlock(g_file_buff_mutex);*/
	}
}

void closeFileStream(){
	fclose(g_file_stream);
}

int getListFile(char *DirPath, char *ListFile){

	DIR *dir = NULL;
	struct dirent *dirnode = NULL;

	dir = opendir(DirPath);
	if(dir){
		while((dirnode = readdir(dir)) != NULL){
			if(dirnode->d_type == DT_REG){
				if((strlen(ListFile) + strlen(dirnode->d_name)) > LIST_FILE_MAX){
					//list file length exceed max len
					return FILE_UNKNOW;
				}
				strcat(ListFile,dirnode->d_name);
				strcat(ListFile,"|");
			}
		}
	}else{
		closedir(dir);
		appLog(LOG_DEBUG, "open directory failed");
		return FILE_ERROR;
	}
	closedir(dir);
	return FILE_SUCCESS;
}
