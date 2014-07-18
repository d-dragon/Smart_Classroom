/*
 * receiveFile_socket.h
 *
 *  Created on: Jul 9, 2014
 *      Author: d-dragon
 */

#ifndef SOCK_INFRA_H_
#define SOCK_INFRA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <ifaddrs.h>

/*define and declare server socket*/
#define PORT 1991
#define BACKLOG	10
#define SOCK_ERROR -1
#define SOCK_SUCCESS 0

int stream_sock_fd, child_stream_sock_fd;
int ret;
struct sockaddr_in serv_addr;
struct sockaddr_in remote_addr;
socklen_t socklen;
size_t num_byte_read;
char file_buff[102400];

int openStreamSocket();


#endif /* SOCK_INFRA_H_ */
