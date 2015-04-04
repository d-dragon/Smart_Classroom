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
#include "xmlHandler.h"
#include <endian.h>
#include <strings.h>

/*Task commands definition*/
struct RequestCmd {
	/*command*/
	const char *cmd_str;
	/* command index*/
	int cmd_index;
	/*number of arguments*/
	int num_args;
	/*arguments*/
	char *args;

} ReqestCmd;

enum request_cmd_index {

	GET_PI_INFO = 1,
	PI_CONNECT_SERVER,
	GET_FILE,
	PLAY_AUDIO,
	STOP_AUDIO,
	PAUSE_AUDIO,
	CLOSE_TASK_HANDLER
};

static struct RequestCmd RequestCmdList[] = {
		{ "GetPiInfo", GET_PI_INFO, 1, "" }, { "PiConnectServer",
				PI_CONNECT_SERVER, 1, "server_ip" }, { "GetFile", GET_FILE, 2,
				"list_file" }, { "PlayFile", PLAY_AUDIO, 1, "file_name" }, {
				"StopFile", STOP_AUDIO, 1, "file_name" }, { "PauseFile",
				PAUSE_AUDIO, 1, "file_name" } };

struct NotifyInfo {
	const char *info_str;
	int info_index;

} NotifyInfo;

enum notify_info_index {
	SERVER_INFO = 1, FTP_ADDR
};
static struct NotifyInfo InfoList[] = { { "ServerInfo", SERVER_INFO }, {
		"ftpaddr", FTP_ADDR } };

ServerInfo server_info;

pthread_mutex_t g_file_buff_mutex;

int stream_sock_sd;

int NotifyMessageHandler(char *message);
int RequestMessageHandler(char *message);
int ResponseMessageHandler(char *message);
int playAudio(char *message);
int getNotifyIndex(char *info);
int getRequestCommandIndex(char *command);

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
		g_remote_addr = inet_ntoa(remote_addr.sin_addr);
		appLog(LOG_INFO, "server: got connection from %s\n", g_remote_addr);
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
//	ret = send(child_stream_sock_fd, "<?xml version=\"1.0\"?><message><type>request</type><action><command>SendFile</command><ftpaddr>192.168.1.34</ftpaddr><username>anonymous</username><filename>21 Guns - Green day.mp3</filename></action></message>", MAX_PACKAGE_LEN, 0);
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
		getFileFromFtp(g_remote_addr, ++ctrlBuff);
//		ret = initFileHandlerThread(ctrlBuff);
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

int initTaskHandler(char *message) {

	int ret;
	char *server_ip = &(g_ServerInfo.serverIp);
	char *room_list;

	room_list = getXmlElementByName(message, "room");
	if (!room_list) {
		appLog(LOG_DEBUG, "server_ip is invalid");
		free(room_list);
		return ACP_FAILED;
	}
	if (strstr(room_list, (char *) ROOM_NAME_DEFAULT) != NULL) {
		ret = pthread_create(&g_TaskHandlerThread, NULL, &TaskHandlerThread,
				(void *) server_ip);
		free(room_list);
		if (ret) {
			appLog(LOG_DEBUG, "init Task Handler failed");
			return ACP_FAILED;
		} else {
			appLog(LOG_DEBUG, "init Task handler success");
			return ACP_SUCCESS;
		}
	}
	free(room_list);
	return ACP_FAILED;
}

void *TaskHandlerThread(void *arg) {

	int ret;
	int byte_count = 0;
	char *server_ip = arg;
	char *pmsg_buff;

	pmsg_buff = calloc(BUFF_LEN_MAX, sizeof(char));

	appLog(LOG_DEBUG, "Pi is connecting to Station at %s", server_ip);
	stream_sock_fd = connecttoStreamSocket(server_ip, STREAM_SOCK_PORT);

	if (stream_sock_fd != SOCK_ERROR) {
		appLog(LOG_DEBUG, "Pi connected to Station!!!");
//		free(server_ip);
	} else {
		appLog(LOG_DEBUG,
				"Pi - Station connection init failed, TaskHandlerThread exit");
//		free(server_ip);
		pthread_exit(NULL);
	}
	sendResultResponse(ACP_SUCCESS, (char *) ROOM_NAME_DEFAULT);
	pthread_mutex_init(&g_audio_status_mutex, NULL);
	while (1) {
		appLog(LOG_DEBUG, "running Task Handler!!!");
		memset(pmsg_buff, 0x00, BUFF_LEN_MAX);

		byte_count = recv(stream_sock_fd, pmsg_buff, BUFF_LEN_MAX, 0);
		if (byte_count < 0) {
			appLog(LOG_DEBUG, "received data failed!");
		} else if (byte_count == 0) {
			appLog(LOG_DEBUG,
					"remote socket was closed -> close stream sock %d",
					stream_sock_fd);
			close(stream_sock_fd);
			free(pmsg_buff);
			appLog(LOG_DEBUG, "exit Task Handler thread!!");
			pthread_exit(NULL);
		} else {
			appLog(LOG_DEBUG, "message data received >>>> %s", pmsg_buff);
			MessageProcessor(pmsg_buff);
		}
	}
}

void MessageProcessor(char *message) {

	int ret;
	char *pmsg_type;

	pmsg_type = getXmlElementByName(message, XML_MESSGAE_TYPE);

	if (pmsg_type != NULL) {
		appLog(LOG_DEBUG, "message type: %s", pmsg_type);
	} else {
		appLog(LOG_DEBUG, "message type is invalid!!");
		free(pmsg_type);
		return;
	}

	if (strcmp(pmsg_type, XML_MESSAGE_NOTIFY) == 0) {
		ret = NotifyMessageHandler(message);
		//TODO
	} else if (strcmp(pmsg_type, XML_MESSAGE_REQUEST) == 0) {
		ret = RequestMessageHandler(message);
		//TODO
	} else if (strcmp(pmsg_type, XML_MESSAGE_RESPONSE) == 0) {
		ret = ResponseMessageHandler(message);
		//TODO
	} else {
		appLog(LOG_DEBUG, "message type not match");
	}
	if (ret == ACP_SUCCESS) {
		appLog(LOG_DEBUG, "Handle %s message success!", pmsg_type);
	} else {
		appLog(LOG_DEBUG, "Handle %s message failed!", pmsg_type);
	}
	free(pmsg_type);
	return;

}

int NotifyMessageHandler(char *message) {

	appLog(LOG_DEBUG, "processing notify message");
	int ret;
	int notify_index;
	char *pinfo;

	pinfo = getXmlElementByName(message, XML_MESSGAE_INFO);
	if (!pinfo) {
		appLog(LOG_DEBUG, "nofity info: %s is invalid", pinfo);
		return ACP_FAILED;
	}
	notify_index = getNotifyIndex(pinfo);

	switch (notify_index) {
	case SERVER_INFO:
		ret = collectServerInfo(message);
		break;
	default:
		appLog(LOG_DEBUG, "notify info %s index %d no match",
				pinfo, notify_index);
		break;
	}

	free(pinfo);
	return ret;
}

int RequestMessageHandler(char *message) {
	appLog(LOG_DEBUG, "processing request message");
	int ret;
	int cmd_index;
	char *cmd;

	cmd = getXmlElementByName(message, XML_MESSAGE_COMMAND);

	if (!cmd) {
		appLog(LOG_DEBUG, "get request command failed: %s", cmd);
		return ACP_FAILED;
	}
	cmd_index = getRequestCommandIndex(cmd);

	if (cmd_index == 0) {
		appLog(LOG_DEBUG, "command not found");
		return ACP_FAILED;
	}
	switch (cmd_index) {
	case PI_CONNECT_SERVER:
		ret = initTaskHandler(message);
		break;
	case GET_FILE:
		ret = getFile(message);
		break;
	case PLAY_AUDIO:
		ret = playAudio(message);
		break;
	case STOP_AUDIO:
		appLog(LOG_DEBUG, "called stopAudio");
		ret = stopAudio();
		break;
	case PAUSE_AUDIO:
		appLog(LOG_DEBUG, "called pauseAudio");
		ret = pauseAudio();
		break;
	default:
		break;
	}
}

int ResponseMessageHandler(char *message) {
	appLog(LOG_DEBUG, "processing response message");
}

int getNotifyIndex(char *info) {

	int numofnotifies = (sizeof(InfoList)) / (sizeof(NotifyInfo));
	int i;
	for (i = 0; i < numofnotifies; i++) {
		appLog(LOG_DEBUG, "InfoList[%d].info_str: %s", i, InfoList[i].info_str);
		if (strcmp(info, InfoList[i].info_str) == 0) {
			return InfoList[i].info_index;
		}
	}
	return 0;
}

int getRequestCommandIndex(char *command) {

	int numofcmd = (sizeof RequestCmdList) / (sizeof ReqestCmd);
	int i;
	for (i = 0; i < numofcmd; i++) {
		if (strcmp(command, RequestCmdList[i].cmd_str) == 0) {
			return RequestCmdList[i].cmd_index;
		}
	}
	return 0;
}

int playAudio(char *message) {

	char *pfile_name;
	int ret;

	if (g_audio_flag == STOP_AUDIO) {
		pfile_name = getXmlElementByName(message, "filename");
		appLog(LOG_DEBUG, "file name: %s", pfile_name);
		if (!pfile_name) {
			appLog(LOG_DEBUG, "file is not exist");
			free(pfile_name);
			sendResultResponse(ACP_FAILED, NULL);
			return ACP_FAILED;
		}
		snprintf(g_file_name_playing, sizeof(g_file_name_playing),"%s",pfile_name);
		ret = initAudioPlayer(pfile_name);
		usleep(500000); //0.5s
		if (ret == ACP_SUCCESS) {
			if (g_audio_flag == AUDIO_PLAY) {
				sendResultResponse(ACP_SUCCESS, pfile_name);
			} else {
				sendResultResponse(ACP_FAILED, NULL);
			}
		}
	} else {

		pfile_name = getXmlElementByName(message, "filename");

		appLog(LOG_DEBUG, "file name: %s", pfile_name);
		if (!pfile_name) {
			appLog(LOG_DEBUG, "file is not exist");
			free(pfile_name);
			sendResultResponse(ACP_FAILED, NULL);
			return ACP_FAILED;
		}
		if(strcmp(g_file_name_playing,pfile_name) == 0){
			appLog(LOG_DEBUG, "%s is playing", pfile_name);
			sendResultResponse(ACP_SUCCESS, "playing");
			return ACP_SUCCESS;
		}
		ret = initAudioPlayer(pfile_name);
		usleep(500000); //0.5s
		if (ret == ACP_SUCCESS) {
			if (g_audio_flag == AUDIO_PLAY) {
				sendResultResponse(ACP_SUCCESS, pfile_name);
			} else {
				sendResultResponse(ACP_FAILED, NULL);
			}
		}
	}
	free(pfile_name);
	return ret;
}
int collectServerInfo( message) {

	char *servIp;
	char *ftpaddr;
	char *ftpusr;
	char *ftppass;

	memset(&(g_ServerInfo.ftp.Password), 0x00,
			sizeof(g_ServerInfo.ftp.Password));

	servIp = getXmlElementByName(message, "serveraddress");
	ftpaddr = getXmlElementByName(message, "ftpaddress");
	ftpusr = getXmlElementByName(message, "ftpuser");
	ftppass = getXmlElementByName(message, "ftppassword");

	if (servIp != NULL) {
		memset(&(g_ServerInfo.serverIp), 0x00, sizeof(g_ServerInfo.serverIp));
		snprintf(g_ServerInfo.serverIp, sizeof(g_ServerInfo.serverIp), "%s",
				servIp);
		free(servIp);
	}

	if (ftpaddr != NULL) {
		memset(&(g_ServerInfo.ftp.Ip), 0x00, sizeof(g_ServerInfo.ftp.Ip));
		snprintf(g_ServerInfo.ftp.Ip, sizeof(g_ServerInfo.ftp.Ip), "%s",
				ftpaddr);
		free(ftpaddr);
	}

	if (ftpusr != NULL) {
		memset(&(g_ServerInfo.ftp.User), 0x00, sizeof(g_ServerInfo.ftp.User));
		snprintf(g_ServerInfo.ftp.User, sizeof(g_ServerInfo.ftp.User), "%s",
				ftpusr);
		free(ftpusr);
	}

	if (ftppass != NULL) {
		memset(&(g_ServerInfo.ftp.Password), 0x00,
				sizeof(g_ServerInfo.ftp.Password));
		snprintf(g_ServerInfo.ftp.Password, sizeof(g_ServerInfo.ftp.Password),
				"%s", ftppass);
		free(ftppass);
	}

	if (!(servIp && ftpaddr && ftpusr && ftppass)) {
		appLog(LOG_DEBUG, "collect Server Info failed");
		return ACP_FAILED;
	}
	appLog(LOG_DEBUG, "%s || %s || %s || %s",
			g_ServerInfo.serverIp, g_ServerInfo.ftp.Ip, g_ServerInfo.ftp.User, g_ServerInfo.ftp.Password);
	char *buff = NULL;
	buff = writeXmlToBuff(RESPONSE_SUCCESS, "room1");
	if (buff == NULL) {
		return ACP_FAILED;
	}
	/*int ret = sendto(multicast_fd, buff, BUFF_LEN_MAX, 0,(struct sockaddr*)&mul_sock, sizeof(mul_sock));
	 if(ret > 0){
	 appLog(LOG_DEBUG, "sent %d byte response success!", ret);
	 }
	 sleep(5);*/
	appLog(LOG_DEBUG, "xml response %d byte: %s", strlen(buff), buff);
	sendMulMessage(buff);
	free(buff);
	return ACP_SUCCESS;
}

int sendResultResponse(int resp_code, char *resp_content) {

	int ret;
	char *resp_buff;

	if (resp_code == 0) {
		resp_buff = writeXmlToBuff((char *) RESPONSE_SUCCESS, resp_content);
	} else {
		resp_buff = writeXmlToBuff((char *) RESPONSE_FAILED, resp_content);
	}

	if (resp_buff == NULL) {
		appLog(LOG_DEBUG, "send result response failed!");
		return ACP_FAILED;
	}

	appLog(LOG_DEBUG, "response content: %s", resp_buff);
	ret = send(stream_sock_fd, resp_buff, strlen(resp_buff), 0);

	if (ret < 0) {
		appLog(LOG_DEBUG, "send response failed");
		return ACP_FAILED;
	}

	return ACP_SUCCESS;
}
