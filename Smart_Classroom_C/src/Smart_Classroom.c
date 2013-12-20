/*
 * Smart_Classroom.c
 *
 *  Created on: Nov 13, 2013
 *      Author: d-dragon
 */
#include "GPIO.h"
#include "Socket.h"
#include "UART.h"

//Define mode control
#define START 1
#define AUTO 2
#define PRESENTATION 3
#define MANUAL 4
#define OFF 5

//Declare MODE comparison String
const char sta[4] = "STA", aut[4] = "AUT", pre[4] = "PRE", man[4] = "MAN",
		off[4] = "OFF";
int flagMan;

int set_Mode(char[]);

void mode_start();
void mode_auto();
void mode_presentation();
void mode_manual();
void mode_off();
void getSensorValue();

int main(void) {

	int i;

//	Init Network
	init_UDPNetwork();
	init_TCPNetwork();
	get_Hostname();
	get_ifaddress();
//Init UART
	init_UART();
	while (1) { // main accept() loop
		printf("SERVER_UDP: waiting for data from client\n");
		printf("IP Address: %s\n", host);
		clientLength = sizeof(client_address);
		message = recvfrom(socketfd_UDP, buf_UDP, sizeof(buf_UDP), 0,
				(struct sockaddr*) &client_address, &clientLength);
		if (message == -1)
			perror("Error: recvfrom call failed");

		printf("SERVER: read %d bytes from IP %s(%s)\n", message,
				inet_ntoa(client_address.sin_addr), buf_UDP);

		if (buf_UDP[0] == 'P' && buf_UDP[1] == '1') {
			message = sendto(socketfd_UDP, host, sizeof(host), 0,
					(struct sockaddr*) &client_address, sizeof(client_address));

			sin_size = sizeof their_addr;

		} else
			continue;

		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		printf("server: got connection from %s\n",
				inet_ntoa(their_addr.sin_addr));
		send(new_fd, "Hello Client\n", 14, 0);
		gpio_export(60); //pin 12
		gpio_export(48); //pin 15
		gpio_set_dir(60, OUTPUT_PIN);
		gpio_set_dir(48, OUTPUT_PIN);
		if (!fork()) { // this is the child process
			while (1) {
				flagMan = 1;
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
					mode_start();
					break;
				case AUTO:
					mode_auto();
					break;
				case PRESENTATION:
					mode_presentation();
					break;
				case MANUAL:
					mode_manual();
					break;
				case OFF:
					mode_off();
					break;
				}

			}
		}
	}
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

void mode_start() {
	int i;
	send(new_fd, "START\n", sizeof "START", 0);
	printf("START\n");

	gpio_set_value(60, LOW);
	usleep(10000);
	gpio_set_value(48, LOW);
	usleep(10000);
	bzero(buf, sizeof buf);

}

void mode_auto() {
	send(new_fd, "AUTO\n", sizeof "AUTO", 0);
	printf("AUTO\n");
	gpio_set_value(60, HIGH);
	usleep(10000);
	gpio_set_value(48, LOW);
	usleep(10000);
	bzero(buf, sizeof buf);
	getSensorValue();
}

void mode_presentation() {
	send(new_fd, "PRESENTATION\n", sizeof "PRESENTATION", 0);
	printf("PRESENTATION\n");
	gpio_set_value(60, LOW);
	usleep(10000);
	gpio_set_value(48, HIGH);
	usleep(10000);
	bzero(buf, sizeof buf);
}

void mode_manual() {
	send(new_fd, "MANUAL\n", sizeof "MANUAL", 0);
	printf("MANUAL\n");
	bzero(buf, sizeof buf);
	while (flagMan) {
		unsigned int temp;
		printf("Proccessing in Manual Mode\n");
		numRead = read(new_fd, buf, sizeof buf);
		if (numRead < 0) {
			perror("Receive Error");
		} else if (numRead == 0) {
			close(new_fd);
			exit(0);
		} else {
			printf("Data received: %s", buf);
		}
		temp = set_Mode(buf);
		if (temp == START || temp == PRESENTATION || temp == AUTO || temp == OFF) {
			flagMan = 0;
			printf("Change Mode\n");
			switch (temp) {
			case START:
				mode_start();
				send(new_fd, "START\n", sizeof "START", 0);
				printf("START\n");
				bzero(buf, sizeof buf);
				break;
			case PRESENTATION:
				mode_presentation();
				send(new_fd, "PRESENTATION\n", sizeof "PRESENTATION", 0);
				printf("PRESENTATION\n");
				bzero(buf, sizeof buf);
				break;
			case AUTO:
				mode_auto();
				send(new_fd, "AUTO\n", sizeof "AUTO", 0);
				printf("AUTO\n");
				bzero(buf, sizeof buf);
				break;
			case OFF:
				mode_off();
				send(new_fd, "OFF\n", sizeof "OFF", 0);
				printf("OFF\n");
				bzero(buf, sizeof buf);
				break;
			}
		} else {
			printf("%s\n", buf);
		}
	}
}

void mode_off() {
	send(new_fd, "OFF\n", sizeof "OFF", 0);
	printf("OFF\n");
	gpio_set_value(60, HIGH);
	usleep(10000);
	gpio_set_value(48, HIGH);
	usleep(10000);
	bzero(buf, sizeof buf);
}

void getSensorValue() {
	write_UART("i");
	read_UART();

	printf("%s", buf_UART);
	bzero(buf_UART, sizeof(buf_UART));
}
