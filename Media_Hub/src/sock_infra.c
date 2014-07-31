/*
 * receiveFile_socket.c
 *
 *  Created on: Jul 9, 2014
 *      Author: d-dragon
 */
#include "sock_infra.h"
#include "logger.h"

/****************************************************
 * open stream socket just for listen connection,	*
 * not include accept() and comunicate task			*
 * **************************************************/
int openStreamSocket(){

	/*Open server socket*/
	stream_sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(stream_sock_fd < 0){
		perror("call socket() error:");
		return SOCK_ERROR;
	}else{
		syslog(LOG_ERR, "Open Sream socket success");
	}

	/* Initialize socket structure */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	socklen = sizeof(remote_addr);

	/*bind socket address to server socket*/
	ret = bind(stream_sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if(ret < 0){
		perror("call bind() error:");
		return SOCK_ERROR;
	}else{
		syslog(LOG_DEBUG, "bind socket success!\n");
	}
	/* Now start listening for the clients, here
	 * process will go in sleep mode and will wait
	 * for the incoming connection
	 */
	ret = listen(stream_sock_fd, BACKLOG);
	if(ret < 0){
		perror("call listen() error");
		return SOCK_ERROR;
	}else{
		syslog(LOG_DEBUG, "TCP socket is listening incoming connection...\n");
	}
	return SOCK_SUCCESS;
}



