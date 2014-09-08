/*
 * advestiment.c
 *
 *  Created on: Aug 5, 2014
 *      Author: duyphan
 */
#include "advertisement.h"
#include "sock_infra.h"
#include "logger.h"

void *advertiseServerInfo() {

	sem_wait(&sem_sock);
	syslog(LOG_DEBUG, "enter advertise server info thread......");
	int ret;
	int num_bytes;
	send_recv_buff = calloc(256, sizeof(char));
	memset(send_recv_buff, 0, sizeof(char));

	//config client address
	udp_client_address.sin_family = AF_INET;
	udp_client_address.sin_port = UDP_PORT;
	udp_client_address.sin_addr.s_addr = inet_addr(BROADCAST_DEST_ADDR);

	ret = openDatagramSocket();
	if (ret < 0) {
		syslog(LOG_ERR, "openDatagramSocket failed!");
		return NULL ;
	} else {
		syslog(LOG_DEBUG, "openDatagramSocket success!");
		sem_destroy(&sem_sock);
		syslog(LOG_DEBUG, "sem_sock was detroyed");
	}

	while (1) {
		syslog(LOG_DEBUG, "server advertise its info......");
		send_recv_buff = AdvPackageWrapper(TCP_PORT, interface_addr);
		num_bytes = sendto(datagram_sock_fd, send_recv_buff,
				(strlen(send_recv_buff)), 0,
				(struct sockaddr *) &udp_client_address,
				sizeof(udp_client_address));
		if(num_bytes < 0 ){
			syslog(LOG_ERR, "sendto call failed");
		}else{
			syslog(LOG_DEBUG, "In advertisement thread sent data success");
			sleep(2);
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

char *AdvPackageWrapper(int port,char *serv_addr){

	syslog(LOG_DEBUG, "call AdvPackageWrapper........");
//	int package_len;
	char *tmp;
	char *tmp_package;

	tmp = calloc(8, sizeof(char));
	tmp_package = calloc(128, sizeof(char));
	memset(tmp_package, 0, sizeof(tmp_package));

	/*start with flag byte set to 1*/
	strcat(tmp_package, "1");

	/*append port number*/
	sprintf(tmp, "%d", port);
	strcat(tmp_package, tmp);
	bzero(tmp,sizeof(tmp));

	/*append length of ip addr and IP*/
	sprintf(tmp, "%d", (int)strlen(serv_addr));
	strcat(tmp_package, tmp);
	strcat(tmp_package, serv_addr);
	syslog(LOG_DEBUG, "Package content: %s length %d", tmp_package, (int)strlen(tmp_package));

	free(tmp);
	return tmp_package;
}
