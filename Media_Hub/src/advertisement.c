/*
 * advestiment.c
 *
 *  Created on: Aug 5, 2014
 *      Author: duyphan
 */
#include "advertisement.h"
#include "acpHandler.h"
#include "sock_infra.h"
#include "logger.h"

void *advertiseServerInfoThread() {

	sem_wait(&sem_sock);
	appLog(LOG_DEBUG, "enter advertise server info thread......\n");
	int ret, count;
	int num_bytes;
	send_recv_buff = calloc(BUFF_LEN_MAX, sizeof(char));
	memset(send_recv_buff, 0, BUFF_LEN_MAX);
	memset(&udp_client_address, 0x00, sizeof(udp_client_address));

	gudp_cli_addr_len = sizeof(udp_client_address);
	//config client address
//	udp_client_address.sin_family = AF_INET;
//	udp_client_address.sin_port = UDP_PORT;
//	udp_client_address.sin_addr.s_addr = inet_addr(interface_addr);

	ret = openDatagramSocket();
	if (ret < 0) {
		appLog(LOG_ERR, "openDatagramSocket failed!\n");
		pthread_exit(NULL);
	} else {
		appLog(LOG_DEBUG, "openDatagramSocket success!\n");
		sem_destroy(&sem_sock);
		appLog(LOG_DEBUG, "sem_sock was detroyed\n");

	}

	while (1) {

		memset(send_recv_buff, 0x00, BUFF_LEN_MAX);
		count = 0;

/*		num_bytes = sendto(datagram_sock_fd, "192.168.168.140",
				strlen("192.168.168.140"), 0,
				(struct sockaddr *) &udp_client_address,
				sizeof(udp_client_address));*/

		udp_byte_read = recvfrom(datagram_sock_fd, send_recv_buff, BUFF_LEN_MAX,
				0, (struct sockaddr*) &udp_client_address, &gudp_cli_addr_len);
		if (udp_byte_read == -1) {
			appLog(LOG_DEBUG, "udp call recvform failed");
			continue;
		} else {
			ret = parseDiscoveredMessage(send_recv_buff);
			if (ret == MESSAGE_INVALID) {
				appLog(LOG_DEBUG, "received invalid udp message!!!");
				continue;
			}

			appLog(LOG_DEBUG, "advertised server info......\n");
			send_recv_buff = AdvPackageWrapper("Room1", interface_addr);
			do {
				count++;
				num_bytes = sendto(datagram_sock_fd, send_recv_buff,
						BUFF_LEN_MAX, 0,
						(struct sockaddr *) &udp_client_address,
						sizeof(udp_client_address));
				appLog(LOG_DEBUG, "bytes sent: %d", num_bytes);
			} while ((num_bytes < 0) && (count <= COUNT_MAX));
			appLog(LOG_DEBUG, "In advertisement thread sent data success\n");
		}
	}

}

/* wrapping advertise package for broadcast device TCP Socket - IP, port
 * package format:
 * 1 byte flag
 * 4 bytes length of the package content
 * 4 bytes for port
 * 4 bytes length of ip address
 * remain byte is ip address
 */

char *AdvPackageWrapper(char *Room, char *serv_addr) {

	appLog(LOG_DEBUG, "inside AdvPackageWrapper........\n");
//	int package_len;
	char *package_tmp;
	unsigned short package_len;

	package_tmp = calloc(BUFF_LEN_MAX, sizeof(char));
	memset(package_tmp, 0, BUFF_LEN_MAX);
	package_tmp[0] = (char) PACKAGE_HEADER;
	package_tmp[1] = (char) PACKAGE_REQ_RESP;
	package_tmp += 2;
	package_len = ((unsigned short) strlen(Room))
			+ ((unsigned short) strlen(serv_addr)) + 1;

	memcpy(package_tmp, &package_len, 2);
	package_tmp += 2;

	/*append Room*/
	strcat(package_tmp, Room);
	strcat(package_tmp, "|"); //append padding

	/*append sever IP*/
	strcat(package_tmp, serv_addr);
	appLog(LOG_DEBUG, "Package content: %s length %d\n",
			package_tmp, (unsigned short)package_len);

	package_tmp -= 4;
	return package_tmp;
}

int parseDiscoveredMessage(char *message) {

	if ((message[0] != PACKAGE_HEADER) && (message[1] != CMD_REQ_DISCOVER)) {
		appLog(LOG_DEBUG, "message[0] %x message[1]", message[0], message[1]);
		return MESSAGE_INVALID;
	}
	return MESSAGE_VALID;
}
