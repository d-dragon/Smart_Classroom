/*
 * receive_file.c
 *
 *  Created on: Jul 12, 2014
 *      Author: d-dragon
 */

#include "receive_file.h"
#include "sock_infra.h"

FILE *createFileStream(char *str_file_name) {

	printf("'start createFileStream()\n");
	FILE *tmp_file;
	char *file_name;
	file_name = calloc(50, sizeof(char));
	file_name = str_file_name;

	printf("FileName: %s\n", file_name);
	printf("create path to file:%s\n", path_to_file);
	strcat(path_to_file, DEFAULT_PATH);
	strcat(path_to_file, file_name);
	printf("path: %s\n", path_to_file);

	printf("create file\n");
	tmp_file = fopen(path_to_file, "w");
	if (tmp_file == NULL) {
		perror("File Error");
		return NULL;
	}
	printf("File stream was created successfully\n");
//	free(file_name);
	return tmp_file;
}

void writetoFileStream() {

	int flag_check_mod = 0;
	FILE *file_store_audio;


	while (1) {

		num_byte_read = read(child_stream_sock_fd, file_buff, sizeof file_buff);
		if (num_byte_read < 0) {
			perror("Receive Error");
		} else if (num_byte_read == 0) {
			printf("client closed\n");
			flag_check_mod = 0;
			close(child_stream_sock_fd);
			exit(0);
		} else {
			//					printf("%d bytes received\n", numRead);
		}
		if (!flag_check_mod) {
			printf("'File Info: %s\n", file_buff);

			if ((file_store_audio = createFileStream((char*) file_buff)) != NULL) {
				if (send(child_stream_sock_fd, "OK", sizeof("OK"), 0) <= 0) {
					perror("Sending error");
				} else {
					printf("Start receive file!\n");
					flag_check_mod = 1;
				}
			} else {
				send(child_stream_sock_fd, "AGAIN", sizeof("AGAIN"), 0);
			}
		} else {
			if (num_byte_read == 3) {
				printf("Close stream file\n");
				if (strcmp(file_buff, "end") == 0) {
					printf("EOF-File Stream closed!\n");
					bzero(file_buff, sizeof file_buff);
					memset(path_to_file, 0, sizeof(path_to_file));
					flag_check_mod = 0;
					fclose(file_store_audio);
				} else {
					printf("File not closed\n");
				}
			}
		}
		if (flag_check_mod) {
			int szwrite = fwrite(file_buff, 1, num_byte_read, file_store_audio);
			//				printf("%d bytes was written\n", szwrite);
			if (szwrite < num_byte_read) {
				perror("File write");
			} else if (szwrite == num_byte_read) {
				printf("Finish receive file session! %d\n", num_byte_read);
				bzero(file_buff, sizeof file_buff);
			}

			if ((num_byte_read == 0)) {
				printf("File transfer complete\n");
				flag_check_mod = 0;
				fclose(file_store_audio);
			}

		}
	}

}
void *recvFileThread() {

	path_to_file = calloc(256, sizeof(char));
	printf("wait for new connection!\n");
	openStreamSocket();
	while (1) {
		child_stream_sock_fd = accept(stream_sock_fd,
				(struct sockaddr *) &remote_addr, &socklen);
		if (child_stream_sock_fd < 0) {
			perror("accept");
			continue;
		}
		printf("server: got connection from %s\n",
				inet_ntoa(remote_addr.sin_addr));

		pid_t pid = fork();
		switch (pid) {
		case 0:
			writetoFileStream();
			break;
		default:
			printf("parent process runing from here\n");
			break;
		}
	}

}

