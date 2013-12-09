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
#include <ifaddrs.h>

//TCP Define
#define PORT "1991"  // the port users will be connecting to
#define PORT_UDP "1992"
#define BACKLOG 10	 // how many pending connections queue will hold
int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
struct addrinfo hints, *servinfo, *p;
struct sockaddr_in their_addr; // connector's address information
socklen_t sin_size;
struct sigaction sa;
struct hostent *hostinfo;
int *ok;
char s[INET6_ADDRSTRLEN], buf[256], service[20];
int rv, hostname, addinfo;
size_t numRead;
char buf_name[512];
struct ifaddrs *ifaddr, *ifa;
int family, ss;
char host[NI_MAXHOST];


//UDP Define
int socketfd_UDP;
struct sockaddr_in server_address, client_address;
char buf_UDP[512];
unsigned int clientLength;
int checkCall, message;
int broadcastEnable;
int ret;


void sigchld_handler(int);
int init_TCPNetwork();
int init_UDPNetwork();
void get_Hostname();
void get_ifaddress();

#endif /* SOCKET_H_ */
