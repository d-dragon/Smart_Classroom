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
 * not include accept() and communication task		*
 * **************************************************/
int openStreamSocket() {

	/*Open server socket*/
	stream_sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (stream_sock_fd < 0) {
		syslog(LOG_ERR, "call socket() error:");
		return SOCK_ERROR;
	} else {
		syslog(LOG_DEBUG, "Open Sream socket success");
	}

	/* Initialize socket structure */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_port = htons(TCP_PORT);
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	socklen = sizeof(remote_addr);

	/*bind socket address to server socket*/
	ret = bind(stream_sock_fd, (struct sockaddr *) &serv_addr,
			sizeof(serv_addr));
	if (ret < 0) {
		syslog(LOG_ERR, "call bind() error:");
		return SOCK_ERROR;
	} else {
		syslog(LOG_DEBUG, "bind socket success!\n");
	}
	/* Now start listening for the clients, here
	 * process will go in sleep mode and will wait
	 * for the incoming connection
	 */
	ret = listen(stream_sock_fd, BACKLOG);
	if (ret < 0) {
		syslog(LOG_ERR, "call listen() error");
		return SOCK_ERROR;
	} else {
		syslog(LOG_DEBUG, "TCP socket is listening incoming connection...\n");
	}
	return SOCK_SUCCESS;
}

int openDatagramSocket() {

	int ret;
	broadcast_enable = 1;
	syslog(LOG_INFO, "Create UDP Broadcast socket......");

	datagram_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (datagram_sock_fd < 0) {
		syslog(LOG_ERR, "Open UDP Broadcast socket failed");
		return SOCK_ERROR;
	} else {
		syslog(LOG_DEBUG, "Open UDP Broadcast socket success");
	}
	ret = setsockopt(datagram_sock_fd, SOL_SOCKET, SO_BROADCAST,
			&broadcast_enable, sizeof(broadcast_enable));
	memset(&udp_server_address, 0, sizeof(udp_server_address));

	//Config UDP Server sock address
	udp_server_address.sin_family = AF_INET;
	udp_server_address.sin_addr.s_addr = htonl(INADDR_ANY );
	udp_server_address.sin_port = htons(atoi(UDP_PORT));
	syslog(LOG_DEBUG, "Config UDP Server sock address success");

	//Bind address to UDP server and listen for incoming client connection
	ret = bind(datagram_sock_fd, (struct sockaddr *) &udp_server_address,
			sizeof(struct sockaddr));
	if(ret < 0 ){
		syslog(LOG_ERR, "bind address to UDP server socket failed!");
		return SOCK_ERROR;
	}else {
		syslog(LOG_DEBUG, "bind address to UDP server socket success!");
	}
	return SOCK_SUCCESS;
}

