/*
 * Socket.h
 *
 *  Created on: Nov 13, 2013
 *      Author: d-dragon
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdio.h>
#include <poll.h>
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


#define PORT "1991"  // the port users will be connecting to
#define BACKLOG 10	 // how many pending connections queue will hold
int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
struct addrinfo hints, *servinfo, *p;
struct sockaddr_in their_addr; // connector's address information
socklen_t sin_size;
struct sigaction sa;
struct hostent *hostinfo;
int *ok;
char s[INET6_ADDRSTRLEN], buf[256], host[1024], service[20];
int rv, hostname, addinfo;
size_t numRead;
char buf_name[512];

void sigchld_handler(int);
int int_Network();
void get_Hostname();
#endif /* SOCKET_H_ */
