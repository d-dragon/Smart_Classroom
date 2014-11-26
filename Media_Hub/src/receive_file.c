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

#include "receive_file.h"
#include "sock_infra.h"
#include "logger.h"
#include "acpHandler.h"

char *initFileInfra(char *FileName) {

	char *tmp_file_path;

}

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

void writetoFileStream() {

	int flag_check_mod = 0;
	FILE *file_store_audio;
	char *buff;
	buff = malloc(32);

	while (1) {

		num_byte_read = read(child_stream_sock_fd, buff, sizeof(buff));
		parsePackageContent(buff);
		//num_byte_read = read(child_stream_sock_fd, file_buff, sizeof file_buff);
		if (num_byte_read < 0) {
			appLog(LOG_ERR, "read() call receive failed!\n");
		} else if (num_byte_read == 0) {
			appLog(LOG_DEBUG, "client connection closed!\n");
			flag_check_mod = 0;
			close(child_stream_sock_fd);
			exit(0);
		} else {
			//					printf("%d bytes received\n", numRead);
		}
		/*
		 if (!flag_check_mod) {
		 appLog(LOG_INFO,"File Info: %s\n", file_buff);

		 if ((file_store_audio = createFileStream((char*) file_buff)) != NULL) {
		 if (send(child_stream_sock_fd, "OK", sizeof("OK"), 0) <= 0) {
		 appLog(LOG_ERR,"sending respond to client failed!\n");
		 } else {
		 appLog(LOG_DEBUG,"Starting receive file>>>>>>>>>>>\n");
		 flag_check_mod = 1;
		 }
		 } else {
		 send(child_stream_sock_fd, "AGAIN", sizeof("AGAIN"), 0);
		 }
		 } else {
		 if (num_byte_read == 3) {
		 appLog(LOG_DEBUG,"Close stream file!\n");
		 if (strcmp(file_buff, "end") == 0) {
		 appLog(LOG_DEBUG, "EOF-File Stream closed!\n");
		 bzero(file_buff, sizeof file_buff);
		 memset(path_to_file, 0, sizeof(char));
		 flag_check_mod = 0;
		 fclose(file_store_audio);
		 } else {
		 appLog(LOG_DEBUG, "File not closed\n");
		 }
		 }
		 }
		 if (flag_check_mod) {
		 int szwrite = fwrite(file_buff, 1, num_byte_read, file_store_audio);
		 //				printf("%d bytes was written\n", szwrite);
		 if (szwrite < num_byte_read) {
		 appLog(LOG_ERR, "write data to stream file failed!\n");
		 } else if (szwrite == num_byte_read) {
		 appLog(LOG_DEBUG,"Finish receive file session! %d\n", (int) num_byte_read);
		 bzero(file_buff, sizeof file_buff);
		 }

		 if ((num_byte_read == 0)) {
		 appLog(LOG_DEBUG, "File transfer complete\n");
		 flag_check_mod = 0;
		 fclose(file_store_audio);
		 }

		 }*/
	}

}
void *recvFileThread() {


	getInterfaceAddress();
	int ret = openStreamSocket();
	if (ret == SOCK_SUCCESS) {
		appLog(LOG_DEBUG, "TCP socket successfully opened--------\n");
		appLog(LOG_DEBUG, "waiting for new connection!\n");
	} else {
		appLog(LOG_DEBUG, "TCP socket open failed\n");
	}
	/*start UDP socket for advertise server info*/
	appLog(LOG_DEBUG, "<call sem_post> to active init UDP sock\n");
	sem_post(&sem_sock);

	while (1) {
		child_stream_sock_fd = accept(stream_sock_fd,
				(struct sockaddr *) &remote_addr, &socklen);
		if (child_stream_sock_fd < 0) {
			appLog(LOG_ERR, "accept call error\n");
			exit(0);
			continue;
		}
		appLog(LOG_INFO, "server: got connection from %s\n",
				inet_ntoa(remote_addr.sin_addr));
		pid_t pid = fork();
		switch (pid) {
		case 0:
			writetoFileStream();
			break;
		default:
			appLog(LOG_DEBUG, "parent process runing from here\n");
			break;
		}
	}

}
void *FileStreamHandlerThread(char *FileInfo) {

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
