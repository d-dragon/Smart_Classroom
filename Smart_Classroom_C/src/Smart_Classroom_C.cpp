//============================================================================
// Name        : Smart_Classroom.cpp
// Author      : PHAN DUY
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
/*
#include <iostream>
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

*********************************************
 * define									  *
 *********************************************

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT 3000
#define MAX_BUF 64

#define PORT "1991"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

enum PIN_DIRECTION{
	INPUT_PIN = 0,
	OUTPUT_PIN= 1
};
enum PIN_VALUE{
	LOW = 0,
	HIGH = 1
};

*********************************************
 * Declare Function							  *
 *********************************************

int gpio_export(unsigned int);
int gpio_unexport(unsigned int);
int gpio_set_dir(unsigned int, PIN_DIRECTION);
int gpio_set_value(unsigned int, PIN_VALUE);
int gpio_get_value(unsigned int,unsigned int*);
int gpio_set_edge(unsigned int, char*);
int gpio_fd_open(unsigned int);
int gpio_fd_close(int);


void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

*********************************************
 * gpio_export								   *
 *********************************************
int gpio_export(unsigned int gpio){

	int fd, len;
	char buf[MAX_BUF];
	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0){
		perror("gpio/export");
		return fd;
	}
	len = snprintf( buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	return 0;
}

*********************************************
 * gpio_unexport								   *
 *********************************************
int gpio_unexport(unsigned int gpio){

	int fd, len;
	char buf[MAX_BUF];
	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0){
		perror("gpio/unexport");
		return fd;
	}
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	return 0;
}

*********************************************
 * gpio_set_dir								   *
 *********************************************
int gpio_set_dir(unsigned int gpio, PIN_DIRECTION out_flag){
	int fd;
	char buf[MAX_BUF];
	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/direction", gpio);

	fd = open(buf , O_WRONLY);
	if (fd < 0){
		perror("gpio/direction");
		return fd;
	}

	if (out_flag == OUTPUT_PIN)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);
	close(fd);
	return 0;
}

*********************************************
 * gpio_set_value							   *
 *********************************************
int gpio_set_value(unsigned int gpio, PIN_VALUE value){

	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

	fd = open(buf, O_WRONLY);

	if (fd < 0){
		perror("gpio/set-value");
		return fd;
	}

	if (value == LOW)
		write(fd, "0", 2);
	else
		write(fd, "1", 2);
	close(fd);
	return 0;

}

*********************************************
 * gpio_get_value							   *
 *********************************************
int gpio_get_value(unsigned int gpio, unsigned int *value){

	int fd;
	char buf[MAX_BUF];
	char ch;

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "gpio%d/value", gpio);

	fd = open(buf, O_WRONLY);
	if(fd < 0){
		perror("gpio/get-value");
		return fd;
	}
	read(fd, &ch, 1);
	if (ch != '0')
		*value = 1;
	else
		*value = 0;
	close(fd);
	return 0;

}

*********************************************
 * gpio_set_edge							  *
 *********************************************
int gpio_set_edge(unsigned int gpio, char *edge){
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);

	fd = open (buf, O_WRONLY);
	if (fd < 0){
		perror("gpio/set-edge");
		return fd;
	}

	write(fd,edge, strlen(edge) + 1);
	close(fd);
	return 0;
}

*********************************************
 * gpio_fd_open							       *
 *********************************************

int gpio_fd_open(unsigned int gpio){

	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
	fd = open(buf, O_RDONLY | O_NONBLOCK);
	if(fd < 0){
		perror("gpio/fd-open");
	}
	return fd;
}

*********************************************
 * gpio_fd_close					     	   *
 *********************************************
int gpio_fd_close(int fd){
	return close(fd);
}

*********************************************
 * MAIN		   					     	      *
 *********************************************
int main(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	struct hostent *hostinfo;
	int yes=1;
	char s[INET6_ADDRSTRLEN], buf[256], host[1024], service[20];
	int rv, hostname,addinfo;
	size_t numRead;
	char buf_name[512];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");
	hostname = gethostname(buf_name, sizeof buf_name);
	if (hostname){
			perror("hostname-error");
		}else
			printf("Server Host Name:%s\n", buf_name);
	if((hostinfo = gethostbyname(buf_name)) == NULL){
		herror("gethostbyname()\n");
	}else{
		printf("Host IP: %s", inet_ntoa(*((struct in_addr *) hostinfo->h_addr_list[0])));
	}
	addinfo = getnameinfo(servinfo->ai_addr, sizeof servinfo->ai_addr, host, sizeof host, service, sizeof service,0);
	if (addinfo == 0){
		perror("getnameinfo");
	}
	else{
		printf("Service:%s\n",service);
	}


	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}
		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);
		send(new_fd, "Hello Client\n", 14, 0);
		gpio_export(60);
		gpio_set_dir(60, OUTPUT_PIN);
		if (!fork()) { // this is the child process
			while(1){

				numRead = read(new_fd, buf, sizeof buf);
				if (numRead < 0){
					perror("Receive Error");
				}else if(numRead == 0){
					close(new_fd);
					exit(0);
				}else{
					printf("Data received: %s", buf);
				}
				if (buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't'){
					send(new_fd, "Disconnected!",sizeof "Disconnected!", 0);
					printf("Client Disconnected!\n");
					close(new_fd);  // parent doesn't need this
					bzero(buf,512); //clear buf
					gpio_unexport(60);
					exit(0);
					break;
				}
				if(buf[0] == 'b' && buf[1] == 'a' && buf[2] == 't'){
					gpio_set_value(60, HIGH);
				}
				if(buf[0] == 't' && buf[1] == 'a' && buf[2] == 't'){
					gpio_set_value(60, LOW);
				}
				bzero(buf,512); //clear buf
			}

		}

	}

	return 0;
}*/
