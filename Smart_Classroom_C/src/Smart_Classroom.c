/*
 * Smart_Classroom.c
 *
 *  Created on: Nov 13, 2013
 *      Author: d-dragon
 */
/*

#include "GPIO.h"
#include "Socket.h"
#include "UART.h"
#include <signal.h>
#include <math.h>

//Define mode control
#define START 1
#define AUTO 2
#define PRESENTATION 3
#define MANUAL 4
#define OFF 5
#define POWER 6
#define MENU 7
#define UP 8
#define DOWN 9

//Declare MODE comparison String
const char sta[4] = "STA", aut[4] = "AUT", pre[4] = "PRE", man[4] = "MAN",
		off[4] = "OFF", poe[4] = "POW", men[4] = "MEN", upp[4] = "UPP", dow[4] =
				"DOW";
int flagMan;
int stop = 0;
pid_t pid_stream;

int set_Mode(char[]);

void mode_start();
void mode_auto();
void mode_presentation();
void mode_manual();
void mode_off();
void getSensorValue();
void stream();
void kill_stream();

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
		gpio_set_value(60, OFFF);
		gpio_set_dir(48, OUTPUT_PIN);
		gpio_set_value(48, OFFF);


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
					//	printf("Data received: %s", buf);
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
	unsigned int temp[10], mode = 0, i;
	temp[0] = temp[1] = temp[2] = temp[3] = temp[4] = temp[5] = temp[6] =
			temp[7] = temp[8] = 0;
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
		if (buf[i] == poe[i]) {
			if (++temp[5] == 3) {
				mode = POWER;
				temp[5] = 0;
			}
			continue;
		}
		if (buf[i] == men[i]) {
			if (++temp[6] == 3) {
				mode = MENU;
				temp[6] = 0;
			}
			continue;
		}
		if (buf[i] == upp[i]) {
			if (++temp[7] == 3) {
				mode = UP;
				temp[7] = 0;
			}
			continue;
		}
		if (buf[i] == dow[i]) {
			if (++temp[8] == 3) {
				mode = DOWN;
				temp[8] = 0;
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

	gpio_set_value(60, ON);
	usleep(10000);
	gpio_set_value(48, ON);
	usleep(10000);
	bzero(buf, sizeof buf);
	stream();

}

void mode_auto() {
	send(new_fd, "AUTO\n", sizeof "AUTO", 0);
	printf("AUTO\n");
	 gpio_set_value(60, HIGH);
	 usleep(10000);
	 gpio_set_value(48, LOW);
	 usleep(10000);
	 bzero(buf, sizeof buf);
	 stop = 0;
	 getSensorValue();
	 printf("%s", buf_UART);

	pid_t child_pid;
	switch (child_pid = fork()) {
	case -1:
		perror("fork");
		break;
	case 0: //child process
		while (1) {
			printf("Child processing\n");
			stop = 0;
			int light = 0, i, n = 0;
			getSensorValue();
			for (i = 6; i >= 4; i--) {
				if (((buf_UART[i] >= '0') && (buf_UART[i] <= '9'))) {
					printf("%c", buf_UART[i]);
					light += pow(10, n) * (buf_UART[i] - '0');
					n++;
				}

			}
			printf("light: %d [lx]", light);
			if (light < 10) {
				gpio_set_value(60, ON);
				usleep(10000);
				gpio_set_value(48, ON);
				usleep(10000);
			} else {
				if (light < 50) {
					gpio_set_value(60, ON);
					usleep(10000);
					gpio_set_value(48, OFFF);
					usleep(10000);
				} else {
					gpio_set_value(60, OFFF);
					usleep(10000);
					gpio_set_value(48,OFFF);
					usleep(10000);
				}

			}
			sleep(1);
		}
		break;
	default: //parent process
		while (flagMan) {
			//		stop = 0;
			//		getSensorValue();

			unsigned int temp;
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
			if (temp == START || temp == PRESENTATION || temp == OFF
					|| temp == MANUAL) {
				flagMan = 0;
				printf("Change Mode\n");
				switch (temp) {
				case START:
					kill(child_pid, SIGKILL);
					mode_start();
					send(new_fd, "START\n", sizeof "START", 0);
					printf("START\n");
					bzero(buf, sizeof buf);

					break;
				case PRESENTATION:
					kill(child_pid, SIGKILL);
					mode_presentation();
					send(new_fd, "PRESENTATION\n", sizeof "PRESENTATION", 0);
					printf("PRESENTATION\n");
					bzero(buf, sizeof buf);
					break;
				case MANUAL:
					kill(child_pid, SIGKILL);
					mode_manual();
					send(new_fd, "MANUAL\n", sizeof "MANUAL", 0);
					printf("MANUAL\n");
					bzero(buf, sizeof buf);
					break;
				case OFF:
					kill(child_pid, SIGKILL);
					mode_off();
					send(new_fd, "OFF\n", sizeof "OFF", 0);
					printf("OFF\n");
					bzero(buf, sizeof buf);
					break;

				}
			}

		}
		break;
	}

//	int a = send(new_fd,buf_UART, sizeof buf_UART, 0);
//	if(a < 0){
//		perror("send");
//	}else
//		printf("send ok");
}

void mode_presentation() {
	send(new_fd, "PRESENTATION\n", sizeof "PRESENTATION", 0);
	printf("PRESENTATION\n");
	gpio_set_value(60, ON);
	usleep(10000);
	gpio_set_value(48, OFFF);
	usleep(10000);
	bzero(buf, sizeof buf);
	write_UART("w");
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
		if (buf[0] == '1') {
			write_UART("1");
			bzero(buf, sizeof buf);
			continue;
		}
		if (buf[0] == '2') {
			write_UART("2");
			bzero(buf, sizeof buf);
			continue;
		}
		if (buf[0] == '3') {
			write_UART("3");
			bzero(buf, sizeof buf);
			continue;
		}
		if (buf[0] == '4') {
			write_UART("4");
			bzero(buf, sizeof buf);
			continue;
		}
		temp = set_Mode(buf);
				if(temp == POWER || temp == MENU || temp == UP || temp == DOWN){
		 switch(temp){
		 case POWER:
		 write_UART("1");
		 bzero(buf, sizeof buf);
		 break;
		 case MENU:
		 write_UART("2");
		 bzero(buf, sizeof buf);
		 break;
		 case UP:
		 write_UART("3");
		 bzero(buf, sizeof buf);
		 break;
		 case DOWN:
		 write_UART("4");
		 bzero(buf, sizeof buf);
		 break;
		 }
		 continue;
		 }else
		 printf("@@\n");
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
	gpio_set_value(60, OFFF);
	usleep(10000);
	gpio_set_value(48, OFFF);
	usleep(10000);
	gpio_set_value(115, OFFF);
	usleep(10000);
	gpio_set_value(117, OFFF);
	usleep(10000);
	bzero(buf, sizeof buf);
	kill_stream();
}

void getSensorValue() {
	bzero(buf_UART, sizeof(buf_UART));

	while (stop == 0) {
		write_UART("i");
		usleep(500000);
		read_UART();
		if (buf_UART[0] == '1' || buf_UART[0] == '2' || buf_UART[0] == '3') {
			stop = 1;
		} else
			sleep(1);
	}
//	printf("OK");

}

void stream() {
	int status;
//	pid_stream = fork();
	if ((pid_stream = fork()) == 0) {
		status =
				system(
						" mjpg_streamer -i \"input_uvc.so -d /dev/video0 -f 30 -r 640x480 \" -o \"output_http.so -p 8085 -w var/www/mjpg-streamer\"");
		_exit(EXIT_SUCCESS); //PID mjpg_streamer = pid_stream + 1
		//		printf(pid_stream);
	}

	printf("\n Now you can do other thing in main process");

	//	return 0;
}

void kill_stream() {
	if (pid_stream) {
		kill(pid_stream, SIGKILL);
		kill(pid_stream + 1, SIGKILL); //kill mjpg_stream
	}

}
*/

