/*
 * acpHandler.c Application Communication Protocol
 *
 *  Created on: Sep 22, 2014
 *      Author: duyphan
 */

#include "acpHandler.h"
#include "logger.h"
#include "sock_infra.h"
#include "receive_file.h"
#include <endian.h>

pthread_mutex_t g_file_buff_mutex;

/*
 pthread_mutex_t g_file_buff_mutex_2 = PTHREAD_MUTEX_INITIALIZER;
 pthread_cond_t	g_file_thread_cond = PTHREAD_COND_INITIALIZER;
 pthread_cond_t	g_file_thread_cond_2 = PTHREAD_COND_INITIALIZER;*/

/*******************************************************
 * This thread is responsible for receive and accept
 * connection from other side application then fork new
 * process for each connection.
 *******************************************************/
void *waitingConnectionThread() {

	int ret;

	getInterfaceAddress();
	ret = openStreamSocket();
	if (ret == SOCK_SUCCESS) {
		appLog(LOG_DEBUG, "TCP socket successfully opened--------\n");
		appLog(LOG_DEBUG, "waiting for new connection!\n");
	} else {
		appLog(LOG_DEBUG, "TCP socket open failed\n");
	}

	/*start UDP socket for advertise server info*/
	appLog(LOG_DEBUG, "<call sem_post> to active init UDP sock\n");
	sem_post(&sem_sock);

	pthread_mutex_init(&g_file_buff_mutex, NULL);
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
			recvnhandlePackageLoop();
			break;
		default:
			appLog(LOG_DEBUG, "parent process runing from here\n");
			break;
		}
	}
}

/*******************************************************
 * Process function used to waiting - receiving and handling
 * the received package
 *******************************************************/
void recvnhandlePackageLoop() {

	int ret;
	char *PackBuff;

	g_RecvFileFlag = 0;

	PackBuff = calloc(MAX_PACKAGE_LEN, sizeof(char));
	g_FileBuff = calloc(MAX_FILE_BUFF_LEN, sizeof(char));

	memset(PackBuff, 0x00, MAX_PACKAGE_LEN);
	memset(g_FileBuff, 0x00, MAX_FILE_BUFF_LEN);

	while (1) {
		/*################ receive package #####################*/
		if (g_RecvFileFlag != RECV_FILE_ENABLED) {
			num_byte_read = read(child_stream_sock_fd, PackBuff,
					MAX_PACKAGE_LEN);
			if (num_byte_read < 0) {
				appLog(LOG_ERR, "read() call receive failed!\n");
			} else if (num_byte_read == 0) {
				appLog(LOG_DEBUG, "client connection closed!\n");
				g_RecvFileFlag = RECV_FILE_DISABLED;
				close(child_stream_sock_fd);
				exit(0);
			} else {
				parsePackageContent(PackBuff);
			}
			memset(PackBuff, 0x00, MAX_PACKAGE_LEN);

		} else {
			/*################ receive data to file here #####################*/
			appLog(LOG_DEBUG, "receiving file data-------");
			pthread_mutex_lock(&g_file_buff_mutex);
			memset(g_FileBuff, 0x00, MAX_FILE_BUFF_LEN);
			num_byte_read = read(child_stream_sock_fd, g_FileBuff,
			MAX_FILE_BUFF_LEN);
			if (num_byte_read < 0) {
				appLog(LOG_ERR, "read() call receive failed!\n");
			} else if (num_byte_read == 0) {
				appLog(LOG_DEBUG, "client connection closed!\n");
				g_RecvFileFlag = RECV_FILE_DISABLED;
				close(child_stream_sock_fd);
				exit(0);
			} else {
				if (isEOFPackage(g_FileBuff)) {
					appLog(LOG_DEBUG, "reached EOF!!!");
					//	closeFileStream();
					g_RecvFileFlag = RECV_FILE_DISABLED;
					g_writeDataFlag = DISABLED;
					pthread_mutex_unlock(&g_file_buff_mutex);
				} else {
					pthread_mutex_unlock(&g_file_buff_mutex);
					g_writeDataFlag = ENABLED;
					while (g_writeDataFlag != DISABLED) {

					}

					//	writeDatatoFile(FileBuff);

				}
			}
		}

	}
}

/*******************************************************
 * Function used for parse package content then call
 * corresponding handler
 *******************************************************/
void parsePackageContent(char *packageBuff) {

	char package_header;
	char package_type;
	short int package_len;
//	int cmd;
//	int num_args;
	int ret, i;
	/*	for (i = 0; i < sizeof(packageBuff); i++) {
	 appLog(LOG_DEBUG, "%x|", packageBuff[i]);
	 }*/
//check whether package header is valid or not (1st byte)
	package_header = *packageBuff;
	appLog(LOG_DEBUG, "package_header %x\n", package_header);
	if (package_header != PACKAGE_HEADER) {
		appLog(LOG_ERR, "received invalid package!!!\n");
		return;
	}
	packageBuff++;

//analyse package type (2nd byte) and take package length
	package_type = *packageBuff;
	appLog(LOG_DEBUG, "package_type %x", package_type);

	memcpy(&package_len, ++packageBuff, sizeof(package_len));

//convert byte order to little endian
	package_len = be16toh(package_len);

	appLog(LOG_DEBUG, "package_len %d\n", package_len);
	packageBuff = packageBuff + 2;

#ifdef DEBUG	//debug--open/
	appLog(LOG_DEBUG, "package_content\n");
	for(i = 0; i < package_len; i++) {
		appLog(LOG_DEBUG, "%x", packageBuff[i]);
	}

#endif 	//debug--close
//parse package type, striped header and package type and length
	switch (package_type) {
	case PACKAGE_CONTROL:
		appLog(LOG_DEBUG, "package type: PACKAGE_CONTROL\n");
		ret = ControlHandler(packageBuff, package_len);
		break;
	case PACKAGE_RESQUEST:
		appLog(LOG_DEBUG, "package type: PACKAGE_REQUEST\n");
		//TODO PACKAGE_REQUEST
		break;
	default:
		appLog(LOG_DEBUG, "package_type invalid\n");
		break;
	}

	return;
}

/************************************************************
 * This handler used for call/process corresponding function
 * for each control command
 ************************************************************/
int ControlHandler(char *ctrlBuff, short int length) {

	appLog(LOG_DEBUG, "inside ControlHandler......\n");
	char *resp;
	resp = calloc(128, sizeof(char));
	int ret;

	switch (*ctrlBuff) {
	case CMD_CTRL_PLAY_AUDIO:

		printf("play audio\n");
		ret = wrapperControlResp((char) CTRL_RESP_SUCCESS);
		break;
	case CMD_SEND_FILE:
		ctrlBuff++;
		ret = initFileHandlerThread(ctrlBuff);
		if (ret == ACP_SUCCESS) {
			ret = wrapperControlResp((char) CTRL_RESP_SUCCESS);
			if (ret == ACP_SUCCESS) {
				return ret;
			}
		} else {
			ret = wrapperControlResp((char) CTRL_RESP_FAILED);
			if (ret == ACP_SUCCESS) {
				return ret;
			}
		}
		break;
	case CMD_START_TRANFER_FILE:
		g_StartTransferFlag = 1;
		break;
	}

	return ret;
}

/*Check if package is EOF control command*/

int isEOFPackage(char *packBuff) {

	char *tmpBuff;
	tmpBuff = calloc(5, sizeof(char));

	memcpy(tmpBuff, packBuff, sizeof(tmpBuff));

	if (*tmpBuff != PACKAGE_HEADER) {
		return 0;
	} else {
		if (*(++tmpBuff) != PACKAGE_CONTROL) {
			return 0;
		} else {
			if (*(tmpBuff + 2) != CMD_CTRL_EOF) {
				return 0;
			}
		}
	}
	free(tmpBuff);
	return 1;
}
int initFileHandlerThread(char *FileInfo) {

	g_StartTransferFlag = 0;
	g_waitCount = 0;

	if (pthread_create(&g_File_Handler_Thd, NULL, &FileStreamHandlerThread,
			&FileInfo)) {
		appLog(LOG_DEBUG, "FileHandlerThread init fail\n");
		return ACP_FAILED;
	}
	pthread_join(&g_File_Handler_Thd, NULL);
	return ACP_SUCCESS;
}
int wrapperControlResp(char resp) {

	char *buff;
	int i, ret;
	buff = calloc(8, sizeof(char));
	memset(buff, 0x00, 8);

	*buff = PACKAGE_HEADER; //header
	*(++buff) = PACKAGE_CTRL_RESP; //package type
	*(++buff) = 0x01; //length
	buff = buff + 2;
	*buff = resp;

//	memcpy(&buff, (char *)resp, sizeof(resp));
	buff = buff - 4;
	ret = send(child_stream_sock_fd, buff, 8, 0);
	if (ret < 0) {
		appLog(LOG_DEBUG, "send response package failed");
		return ACP_FAILED;
	}

	//debug package response
	for (i = 0; i < 8; i++) {
		appLog(LOG_DEBUG, "%x", buff[i]);
	}
	free(buff);
	appLog(LOG_DEBUG, "send response package success");
	return ACP_SUCCESS;
}
int wrapperRequestResp(char *resp) {
	char *buff;
	int i, ret;
	buff = calloc(8, sizeof(char));
	memset(buff, 0x00, 8);

	*buff = PACKAGE_HEADER; //header
	*(++buff) = PACKAGE_CTRL_RESP; //package type
	*(++buff) = 0x01; //length
	buff = buff + 2;
	*buff = *resp;

	//	memcpy(&buff, (char *)resp, sizeof(resp));
	buff = buff - 4;
	ret = send(child_stream_sock_fd, buff, 8, 0);
	if (ret < 0) {
		appLog(LOG_DEBUG, "send response package failed");
		return ACP_FAILED;
	}

	//debug package response
	for (i = 0; i < 8; i++) {
		appLog(LOG_DEBUG, "%x", buff[i]);
	}
	free(buff);
	appLog(LOG_DEBUG, "send response package success");
	return ACP_SUCCESS;
}

