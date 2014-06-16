/*
 ============================================================================
 Name        : EnglishTest.c
 Author      : PHAN DUY
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "Socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define SUCCESS 1
#define ERROR 0
#define DEFAULTPATH "/home/root/AudioEnglishTest/"

char *pathtoFile;
FILE *FileReceived;

int createFileStream(char *);

int main() {

	printf("Init Inet socket\n");
	init_TCPNetwork();
	get_Hostname();
	get_ifaddress();

	char buf[10000];

	int flag = 0;

	pathtoFile = calloc(100, sizeof(char));

	printf("wait new connection!\n");
	while (1) {

		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}
		printf("server: got connection from %s\n",
				inet_ntoa(their_addr.sin_addr));

		pid_t pid = fork();
		switch (pid) {
		case 0:
			while (1) {

				numRead = read(new_fd, buf, sizeof buf);
				if (numRead < 0) {
					perror("Receive Error");
				} else if (numRead == 0) {
					printf("client closed\n");
					flag = 0;
					close(new_fd);
					exit(0);
				} else {
//					printf("%d bytes received\n", numRead);
				}
				if (!flag) {
					printf("'File Info: %s\n", buf);

					if (createFileStream((char*) buf) == SUCCESS) {
						if (send(new_fd, "OK", sizeof("OK"), 0) <= 0) {
							perror("Sending error");
						}else{
						flag = 1;
						continue;
						}
					} else {
						send(new_fd, "AGAIN", sizeof("AGAIN"),0);
					}
				}

				int szwrite = fwrite(buf, 1, numRead, FileReceived);
//				printf("%d bytes was written\n", szwrite);
				if (szwrite < numRead) {
					perror("File write");
				} else if (szwrite == numRead)
					bzero(buf, sizeof buf);
				if ((numRead == 0)) {
					printf("File transfer complete\n");
					flag = 0;
					fclose(FileReceived);
				}

			}
			break;

		}

	}
	return 0;
}

int createFileStream(char *StrFileInfo) {

	printf("'start createFileStream()\n");
	char *FileName;
	int FileNameLenght;
	FileName = calloc(50, sizeof(char));
	printf("cal file lenght\n");
	FileNameLenght = strlen(StrFileInfo) - strlen(strchr(StrFileInfo, ' '));

	printf("create file name\n");
	strncpy(FileName, StrFileInfo, FileNameLenght);
	printf("FileName: %s\n", FileName);
	printf("create path to file:%s\n", pathtoFile);
	strcat(pathtoFile, DEFAULTPATH);
	strcat(pathtoFile, FileName);
//	strcat(pathtoFile, ".mp3");
	printf("path: %s\n", pathtoFile);

	printf("create file\n");
	FileReceived = fopen(pathtoFile, "w");
	if (FileReceived == NULL) {
		perror("File Error");
		return ERROR;
	} else {
		printf("File was created successfully\n");
		return SUCCESS;
	}

}
