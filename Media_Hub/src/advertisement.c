/*
 * advestiment.c
 *
 *  Created on: Aug 5, 2014
 *      Author: duyphan
 */
#include "advertisement.h"
#include "sock_infra.h"
#include "logger.h"

void *advertise_server_info() {

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
		send_recv_buff = "server_info";
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

