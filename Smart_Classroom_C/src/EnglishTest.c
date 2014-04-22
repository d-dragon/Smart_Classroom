/*
 ============================================================================
 Name        : EnglishTest.c
 Author      : PHAN DUY
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
/*

#include "Socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main() {

	init_TCPNetwork();
	get_Hostname();
	get_ifaddress();
	char *file_recv = "/home/d-dragon/received.mp3";
	FILE *fr = fopen(file_recv, "w+");
	if (fr == NULL) {
		perror("File Error");
	} else
		printf("File was created successfully\n");

	while (1) {

		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}
		printf("server: got connection from %s\n",
				inet_ntoa(their_addr.sin_addr));

		int flag = 0;
		pid_t pid = fork();
		switch (pid) {
		case 0:
			while (1) {

				numRead = read(new_fd, buf, sizeof buf);
				if (numRead < 0) {
					perror("Receive Error");
				} else if (numRead == 0) {
					fclose(fr);
					close(new_fd);
					exit(0);
				} else {
					printf("%d bytes received: %d\n", numRead);
					//	printf("Data received: %s\n", buf);

				}

//				if (flag == 1) {
					int szwrite = fwrite(buf, 1, numRead, fr);
					printf("%d bytes was written\n", szwrite);
					if (szwrite < numRead) {
						perror("File write");
					} else if (szwrite == numRead)
						printf("File was wrote successfully\n");
					bzero(buf, sizeof buf);
//				}

//				if (flag == 0) {
//					char buffer[atoi(buf)];
//					printf("Size of buffer %d", sizeof buffer);
//					flag = 1;
//				}

			}
			break;

		}

	}
	return 0;
}
*/
