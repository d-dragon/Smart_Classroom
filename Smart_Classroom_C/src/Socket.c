/*
 * Socket.c
 *
 *  Created on: Nov 13, 2013
 *      Author: d-dragon
 */

#include "Socket.h"

void sigchld_handler(int s) {
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
}

int init_Network(void) {
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	ok = 1;
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &ok, sizeof(int))
				== -1) {
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
	if (p == NULL) {
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
	return 1;
}
void get_Hostname() {
	hostname = gethostname(buf_name, sizeof buf_name);
	if (hostname) {
		perror("hostname-error");
	} else
		printf("Server Host Name:%s\n", buf_name);
	if ((hostinfo = gethostbyname(buf_name)) == NULL) {
		herror("gethostbyname()\n");
	} else {
		printf("Host IP: %s",
				inet_ntoa(*((struct in_addr *) hostinfo->h_addr_list[0])));
	}
}
// get sockaddr, IPv4 or IPv6:
//void *get_in_addr(struct sockaddr *sa)
//{
//	if (sa->sa_family == AF_INET) {
//		return &(((struct sockaddr_in*)sa)->sin_addr);
//	}
//
//	return &(((struct sockaddr_in6*)sa)->sin6_addr);
//}
