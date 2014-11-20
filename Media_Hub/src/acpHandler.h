/*
 * acpHandler.h  Application Communication Protocol
 *
 *  Created on: Sep 22, 2014
 *      Author: duyphan
 */

#ifndef ACPHANDLER_H_
#define ACPHANDLER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define ACP_SUCCESS		1
#define ACP_FAILED		0
#define MAX_FILE_BUFF_LEN	100*1024
#define RECV_FILE_ENABLED	1
#define RECV_FILE_DISABLED 0
#define MAX_WAITING_COUNT 	3000
#define ENABLED		1
#define DISABLED	0

/********************************************************
 * Define communication protocol session
 ********************************************************/
#define MAX_PACKAGE_LEN		512
#define PACKAGE_HEADER 		0x55 //package header

//package type
#define PACKAGE_CONTROL 	0x01
#define PACKAGE_RESQUEST 	0x02
#define PACKAGE_CTRL_RESP  0x03
#define PACKAGE_REQ_RESP	0x04

//define command control
#define CMD_CTRL_PLAY_AUDIO 		0x00
#define CMD_CTRL_STOP_AUDIO			0x01
#define CMD_SEND_FILE				0x02
#define CMD_START_TRANFER_FILE		0x03
#define CMD_CTRL_EOF				0x04

//define request
#define CMD_REQ_GET_LIST_FILE		0x01

//define control response
#define CTRL_RESP_SUCCESS			0x01
#define CTRL_RESP_FAILED			0x00
#define CTRL_RESP_ALREADY			0x02
/******************************************************/

int g_RecvFileFlag;
int g_StartTransferFlag;
int g_waitCount;
int g_writeDataFlag;

char *g_FileBuff;
typedef char byte;

typedef struct PackageContent{
	byte package_code;
	short int num_arg;
	byte *args;
}PackageContent;

typedef struct PackageData{
	byte header;
	byte type;
	short int length;
	PackageContent content;
}PackageData;


//Package function declaration
void parsePackageContent(char *packageBuff);
int wrapperControlResp(char resp);
int wrapperRequestResp(char *resp);
int ControlHandler(char *ctrlBuff, short int length);

void *waitingConnectionThread();
void recvnhandlePackageLoop();
int isEOFPackage(char *);
int initFileHandlerThread(char *);

#endif /* ACP_H_ */




/*
 *
 * PLAYAUDIOFLAG=-D PLAY_AUDIO
.c.o: $(DEPS)
			gcc $(CFLAGS) -c -o $@  $<
*/
