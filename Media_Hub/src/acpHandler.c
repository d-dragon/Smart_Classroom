/*
 * acpHandler.c Application Communication Protocol
 *
 *  Created on: Sep 22, 2014
 *      Author: duyphan
 */

#include "acpHandler.h"
#include "logger.h"
#include "sock_infra.h"
#include "FileHandler.h"
#include "playAudio.h"
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
	appLog(LOG_DEBUG, "PackBuff addr: %p", PackBuff);
	pthread_mutex_init(&g_audio_status_mutex, NULL);

	while (1) {
		/*################ receive message package here#####################*/
		if (g_RecvFileFlag != RECV_FILE_ENABLED) {
			appLog(LOG_DEBUG, "receiving message----");
			num_byte_read = read(child_stream_sock_fd, PackBuff,
					MAX_PACKAGE_LEN);
			if (num_byte_read < 0) {
				appLog(LOG_ERR, "read() call receive failed!\n");
			} else if (num_byte_read == 0) {
				appLog(LOG_DEBUG, "client connection closed!\n");
				g_RecvFileFlag = RECV_FILE_DISABLED;

				free(PackBuff);
				free(g_FileBuff);
				close(child_stream_sock_fd);
				exit(0);
			} else {
				parsePackageContent(PackBuff);
			}
			memset(PackBuff, 0x00, MAX_PACKAGE_LEN);

		} else {
			/*################ receive file data here #####################*/
			if (g_writeDataFlag != DISABLED) {
				continue;
			}
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

					g_RecvFileFlag = RECV_FILE_DISABLED;
					g_writeDataFlag = ENABLED; //force stop FileStreamHandlerThread
					memset(g_FileBuff, 0x00, MAX_FILE_BUFF_LEN);
					pthread_mutex_unlock(&g_file_buff_mutex);
				} else {
					appLog(LOG_DEBUG, "enabled write data to file")
					g_writeDataFlag = ENABLED;
					pthread_mutex_unlock(&g_file_buff_mutex);
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
	int i;
	/*	for (i = 0; i < sizeof(packageBuff); i++) {
	 appLog(LOG_DEBUG, "%x|", packageBuff[i]);
	 }*/
//check whether package header is valid or not (1st byte)
	package_header = *packageBuff;
//	appLog(LOG_DEBUG, "package_header %x\n", package_header);
	if (package_header != PACKAGE_HEADER) {
		appLog(LOG_ERR, "received invalid package!!!\n");
		return;
	}
	packageBuff++;

//analyse package type (2nd byte) and take package length
	package_type = *packageBuff;
//	appLog(LOG_DEBUG, "package_type %x", package_type);

	memcpy(&package_len, ++packageBuff, sizeof(package_len));

//convert byte order to little endian
	package_len = be16toh(package_len);

//	appLog(LOG_DEBUG, "package_len %d\n", package_len);
	packageBuff = packageBuff + 2;

	/*
	 #ifdef DEBUG	//debug--open/
	 appLog(LOG_DEBUG, "package_content\n");
	 for(i = 0; i < package_len; i++) {
	 appLog(LOG_DEBUG, "%x", packageBuff[i]);
	 }

	 #endif 	//debug--close
	 */
//parse package type, striped header and package type and length
	switch (package_type) {
	case PACKAGE_CONTROL:
		appLog(LOG_DEBUG, "package type: PACKAGE_CONTROL\n");
		ret = ControlHandler(packageBuff, package_len);
		break;
	case PACKAGE_RESQUEST:
		appLog(LOG_DEBUG, "package type: PACKAGE_REQUEST\n");
		//TODO PACKAGE_REQUEST
		ret = RequestHandler(packageBuff);
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
	int ret;
	switch (*ctrlBuff) {
	case CMD_CTRL_PLAY_AUDIO:
		appLog(LOG_DEBUG, "CMD_CTRL_PLAY_AUDIO");
		if (g_audio_flag == AUDIO_PAUSE) {
			pthread_mutex_lock(&g_audio_status_mutex);
			g_audio_flag = AUDIO_PLAY;
			pthread_mutex_unlock(&g_audio_status_mutex);
			wrapperControlResp((char) CTRL_RESP_SUCCESS);
		} else {
			ret = initAudioPlayer(++ctrlBuff);
			ret = wrapperControlResp((char) CTRL_RESP_SUCCESS);
		}
		break;
	case CMD_CTRL_STOP_AUDIO:
		appLog(LOG_DEBUG, "CMD_CTRL_STOP_AUDIO");
		ret = stopAudio();
		if (ret == ACP_SUCCESS) {
			wrapperControlResp((char) CTRL_RESP_SUCCESS);
		}
		break;
	case CMD_CTRL_PAUSE_AUDIO:
		appLog(LOG_DEBUG, "CMD_CTRL_PAUSE_AUDIO");
		ret = pauseAudio();
		if (ret == ACP_SUCCESS) {
			wrapperControlResp((char) CTRL_RESP_SUCCESS);
		}
		break;
	case CMD_SEND_FILE:
		appLog(LOG_DEBUG, "CMD_SEND_FILE");
		getFileFromFtp("10.0.0.100", "IMG_3858.CR2");
		ret = initFileHandlerThread(++ctrlBuff);
		if (ret == ACP_SUCCESS) {
			ret = wrapperControlResp(CTRL_RESP_ALREADY);
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
		appLog(LOG_DEBUG, "CMD_START_TRANFER_FILE");
		g_StartTransferFlag = 1;
		break;
	default:
		appLog(LOG_DEBUG, "Command invalid");
		break;
	}

	return ret;
}

int RequestHandler(char *reqBuff) {

	int ret;
	switch (*reqBuff) {
	case CMD_REQ_GET_LIST_FILE:
		appLog(LOG_DEBUG, "CMD_REQ_GET_LIST_FILE");
		char *DirPath;
		char *ListFile;

		DirPath = malloc(FILE_PATH_LEN_MAX * sizeof(char));
		ListFile = malloc(LIST_FILE_MAX * sizeof(char));
		appLog(LOG_DEBUG, "DirPath addr: %p || ListFile addr: %p",
				DirPath, ListFile);
		memset(DirPath, 0, FILE_PATH_LEN_MAX);
		memset(ListFile, 0, LIST_FILE_MAX);

		strcat(DirPath, (char *) DEFAULT_PATH);
		appLog(LOG_DEBUG, "DirPath: %s", DirPath);

		ret = getListFile(DirPath, ListFile);
		if (ret == FILE_SUCCESS) {
			appLog(LOG_DEBUG, "%s", ListFile);
			ret = wrapperRequestResp(ListFile);
			appLog(LOG_DEBUG, "DirPath addr: %p || ListFile addr: %p",
					DirPath, ListFile);
			free(DirPath);
			free(ListFile);
			return ret;
		} else {
			free(DirPath);
			free(ListFile);
			return ACP_FAILED;
		}
		//no need break;
	}
}
/*Check if package is EOF control command*/

static int isEOFPackage(char *packBuff) {

	char tmpBuff[5];
	memcpy(&tmpBuff, packBuff, 5);
	if ((tmpBuff[0] != PACKAGE_HEADER) || (tmpBuff[1] != PACKAGE_CONTROL)
			|| (tmpBuff[4] != CMD_CTRL_EOF)) {
		return 0;
	}
	appLog(LOG_DEBUG, "isEOF");
	return 1;
}

int initFileHandlerThread(char *FileInfo) {

	g_StartTransferFlag = 0;
	g_waitCount = 0;
	g_file_stream = createFileStream(FileInfo);
	if (g_file_stream == NULL) {
		appLog(LOG_DEBUG, "Create file stream failed!!!");
		return ACP_FAILED;
	}
	if (pthread_create(&g_File_Handler_Thd, NULL, &FileStreamHandlerThread,
			NULL)) {
		appLog(LOG_DEBUG, "FileHandlerThread init fail\n");
		return ACP_FAILED;
	}
	pthread_mutex_init(&g_file_buff_mutex, NULL);
	g_RecvFileFlag = RECV_FILE_ENABLED;
//	pthread_join(&g_File_Handler_Thd, NULL);
	return ACP_SUCCESS;
}

int initAudioPlayer(char *filename) {

	/*FileInfo *file;
	 file = malloc(sizeof(FileInfo));
	 file->filename = malloc(100);
	 file->filename = "m.mp3";
	 file->index = 0;*/
	appLog(LOG_DEBUG, "begin %s", __FUNCTION__);
	char *FileName = calloc(FILE_NAME_MAX, sizeof(char));
	if (FileName == NULL) {
		appLog(LOG_DEBUG, "allocated memory failed");
		return ACP_FAILED;
	} else {
		appLog(LOG_DEBUG, "allocated memory success");
	}
//	appLog(LOG_DEBUG, "address FileName: %p", FileName);
	memset(FileName, 0, FILE_NAME_MAX);
	//  strlen -1 to truncate '|' charater at end of string
	strncat(FileName, filename, strlen(filename) - 1); //cann't assign FileName = "m.mp3", it change pointer address -> can't free()

	/*Need to parse file index to get file name*/

	//init play audio thread
	pthread_mutex_lock(&g_audio_status_mutex);
	g_audio_flag = AUDIO_STOP;
	pthread_mutex_unlock(&g_audio_status_mutex);

	//FileName will be freed in playAudioThread
	if (pthread_create(&g_play_audio_thd, NULL, &playAudioThread,
			(void *) FileName)) {
		appLog(LOG_DEBUG, "init playAudioThread failed!!!");
		return ACP_FAILED;
	}
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
	unsigned char *buff;
	int i, ret;
	unsigned int resp_len = (unsigned int) strlen(resp);
	resp_len += 1;
	buff = malloc(MAX_PACKAGE_LEN);
	memset(buff, 0x00, MAX_PACKAGE_LEN);

	*buff = (char) PACKAGE_HEADER; //header
	buff++;
	*buff = (char) PACKAGE_REQ_RESP; //package type
	buff++;
	appLog(LOG_DEBUG, "content length: %d", resp_len);
	memcpy(buff, (void *) &resp_len, 2); //content length
	appLog(LOG_DEBUG, "content length: %d", resp_len);
	buff += 2;
	*buff = (char) REQ_RESP_GET_LIST;
	buff++;
	memcpy(buff, resp, strlen(resp));

	//	memcpy(&buff, (char *)resp, sizeof(resp));
	buff -= 5;
	ret = send(child_stream_sock_fd, buff, MAX_PACKAGE_LEN, 0);
	if (ret < 0) {
		appLog(LOG_DEBUG, "send response package failed");
		return ACP_FAILED;
	}

#ifdef DEBUG
	//debug package response
	for (i = 0; i < 5; i++) {
		appLog(LOG_DEBUG, "%x", buff[i]);
	}
	buff += 5;
	appLog(LOG_DEBUG, "content:%s", buff);
	buff -= 5;
#endif

//	free(resp);
	free(buff);
	appLog(LOG_DEBUG, "send response package %d success", ret);
	return ACP_SUCCESS;
}

