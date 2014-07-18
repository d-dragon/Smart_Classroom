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

int init_TCPNetwork(void) {
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

int init_UDPNetwork() {
	broadcastEnable = 1;
	/*Create socket */
	socketfd_UDP = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketfd_UDP == -1)
		perror("Error: socket failed");

	ret = setsockopt(socketfd_UDP, SOL_SOCKET, SO_BROADCAST, &broadcastEnable,
			sizeof(broadcastEnable));
	bzero((char*) &server_address, sizeof(server_address));

	/*Fill in server's sockaddr_in*/
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(atoi(PORT_UDP));

	/*Bind server socket and listen for incoming clients*/
	checkCall = bind(socketfd_UDP, (struct sockaddr *) &server_address,
			sizeof(struct sockaddr));
	if (checkCall == -1)
		perror("Error: bind call failed");
	return 1;
	/*	while (1) {
	 printf("SERVER_UDP: waiting for data from client\n");

	 clientLength = sizeof(client_address);
	 message = recvfrom(socketfd_UDP, buf_UDP, sizeof(buf_UDP), 0,
	 (struct sockaddr*) &client_address, &clientLength);
	 if (message == -1)
	 perror("Error: recvfrom call failed");

	 printf("SERVER: read %d bytes from IP %s(%s)\n", message,
	 inet_ntoa(client_address.sin_addr), buf_UDP);

	 if (!strcmp(buf_UDP, "quit"))
	 break;

	 strcpy(buf_UDP, "ok");

	 message = sendto(socketfd_UDP, buf_UDP, strlen(buf_UDP) + 1, 0,
	 (struct sockaddr*) &client_address, sizeof(client_address));
	 if (message == -1)
	 perror("Error: sendto call failed");

	 printf("SERVER: send completed\n");
	 }
	 checkCall = close(socketfd_UDP);
	 if (checkCall == -1)
	 perror("Error: bind call failed");*/

}
void get_ifaddress() {
	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}
	const char *s;

	/* Walk through linked list, maintaining head pointer so we
	 can free list later */

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;

		family = ifa->ifa_addr->sa_family;

		/* Display interface name and family (including symbolic
		 form of the latter for the common families) */

		printf("%s  address family: %d%s\n", ifa->ifa_name, family,
				(family == AF_PACKET) ? " (AF_PACKET)" :
				(family == AF_INET) ? " (AF_INET)" :
				(family == AF_INET6) ? " (AF_INET6)" : "");

		/* For an AF_INET* interface address, display the address */
		s = ifa->ifa_name;
//		printf("%s",s);
		if (family == AF_INET && s[0] == 'e') {
			ss = getnameinfo(ifa->ifa_addr,
					(family == AF_INET) ?
							sizeof(struct sockaddr_in) :
							sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL,
					0, NI_NUMERICHOST);
			if (ss != 0) {
				printf("getnameinfo() failed: %s\n", gai_strerror(ss));
				exit(EXIT_FAILURE);
			}
			printf("\taddress: <%s>\n", host);
		}
	}
}

