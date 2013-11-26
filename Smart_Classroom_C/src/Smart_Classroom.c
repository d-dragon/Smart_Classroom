/*
 * Smart_Classroom.c
 *
 *  Created on: Nov 13, 2013
 *      Author: d-dragon
 */
#include "GPIO.h"
#include "Socket.h"

//Define mode control
#define START 1
#define AUTO 2
#define PRESENTATION 3
#define MANUAL 4
#define OFF 5

//Declare MODE comparison String
const char sta[4] = "STA", aut[4] = "AUT", pre[4] = "PRE", man[4] = "MAN",
		off[4] = "OFF";

int set_Mode(char[]);

int main(void) {
//	Init comparison string

//	Init Network
	init_Network();
	get_Hostname();

	addinfo = getnameinfo(servinfo->ai_addr, sizeof servinfo->ai_addr, host,
			sizeof host, service, sizeof service, 0);
	if (addinfo == 0) {
		perror("getnameinfo");
	} else {
		printf("Service:%s\n", service);
	}

	while (1) { // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}
//		inet_ntop(their_addr.ss_family,
//			get_in_addr((struct sockaddr *)&their_addr),
//			s, sizeof s);
		printf("server: got connection from %s\n",
				inet_ntoa(their_addr.sin_addr));
		send(new_fd, "Hello Client\n", 14, 0);
		gpio_export(60);
		gpio_set_dir(60, OUTPUT_PIN);
		if (!fork()) { // this is the child process
			while (1) {
				int flagMan = 1;
				numRead = read(new_fd, buf, sizeof buf);
				if (numRead < 0) {
					perror("Receive Error");
				} else if (numRead == 0) {
					close(new_fd);
					exit(0);
				} else {
					printf("Data received: %s", buf);
				}

				switch (set_Mode(buf)) {
				case START:
					send(new_fd, "START\n", sizeof "START", 0);
					printf("START\n");
					bzero(buf, sizeof buf);
					break;
				case AUTO:
					send(new_fd, "AUTO\n", sizeof "AUTO", 0);
					printf("AUTO\n");
					bzero(buf, sizeof buf);
					break;
				case PRESENTATION:
					send(new_fd, "PRESENTATION\n", sizeof "PRESENTATION", 0);
					printf("PRESENTATION\n");
					bzero(buf, sizeof buf);
					break;
				case MANUAL:
					send(new_fd, "MANUAL\n", sizeof "MANUAL", 0);
					printf("MANUAL\n");
					bzero(buf, sizeof buf);
					while (flagMan) {
						unsigned int temp;
						printf("Proccessing in Manual Mode\n");
						numRead = read(new_fd, buf, sizeof buf);
						temp = set_Mode(buf);
						if (temp == START || temp == PRESENTATION
								|| temp == AUTO || temp == OFF) {
							flagMan = 0;
							printf("Change Mode\n");
							switch (temp) {
							case START:
								send(new_fd, "START\n", sizeof "START", 0);
								printf("START\n");
								bzero(buf, sizeof buf);
								break;
							case PRESENTATION:
								send(new_fd, "PRESENTATION\n",
										sizeof "PRESENTATION", 0);
								printf("PRESENTATION\n");
								bzero(buf, sizeof buf);
								break;
							case AUTO:
								send(new_fd, "AUTO\n", sizeof "AUTO", 0);
								printf("AUTO\n");
								bzero(buf, sizeof buf);
								break;
							case OFF:
								send(new_fd, "OFF\n", sizeof "OFF", 0);
								printf("OFF\n");
								bzero(buf, sizeof buf);
								break;
							}
						} else {
							printf("%s\n", buf);
						}
					}
					break;
				case OFF:
					send(new_fd, "OFF\n", sizeof "OFF", 0);
					printf("OFF\n");
					bzero(buf, sizeof buf);
					break;
				}
				/*if (buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i'
				 && buf[3] == 't') {
				 send(new_fd, "Disconnected!", sizeof "Disconnected!", 0);
				 printf("Client Disconnected!\n");
				 close(new_fd); // parent doesn't need this
				 bzero(buf, 512); //clear buf
				 gpio_unexport(60);
				 exit(0);
				 break;
				 }
				 if (buf[0] == 'b' && buf[1] == 'a' && buf[2] == 't') {
				 gpio_set_value(60, HIGH);
				 }
				 if (buf[0] == 't' && buf[1] == 'a' && buf[2] == 't') {
				 gpio_set_value(60, LOW);
				 }
				 bzero(buf, 512); //clear buf*/
			}

		}

	}

	return 0;
}
int set_Mode(char buf[]) {
	unsigned int temp[6], mode = 0, i;
	temp[0] = temp[1] = temp[2] = temp[3] = temp[4] = 0;
	for (i = 0; i < 3; i++) {
		if (buf[i] == sta[i]) {
			if (++temp[0] == 3) {
				mode = START;
				temp[0] = 0;
			}
			continue;
		}
		if (buf[i] == aut[i]) {
			if (++temp[1] == 3) {
				mode = AUTO;
				temp[1] = 0;
			}
			continue;
		}
		if (buf[i] == pre[i]) {
			if (++temp[2] == 3) {
				mode = PRESENTATION;
				temp[2] = 0;
			}
			continue;
		}
		if (buf[i] == man[i]) {
			if (++temp[3] == 3) {
				mode = MANUAL;
				temp[3] = 0;
			}
			continue;
		}
		if (buf[i] == off[i]) {
			if (++temp[4] == 3) {
				mode = OFF;
				temp[4] = 0;
			}
			continue;
		}
	}
	return mode;
}

