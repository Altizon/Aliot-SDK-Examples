/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "MQTTMbed.h"
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <wm_os.h>
#include <string.h>

typedef int tls_handle_t;

typedef enum {
	/* TLS server mode */
	/* If this flag bit is zero client mode is assumed.*/
	TLS_SERVER_MODE = 0x01,
	TLS_CHECK_CLIENT_CERT = 0x02,

	/* TLS Client mode */
	TLS_CHECK_SERVER_CERT = 0x04,
	/* This will be needed if server mandates client certificate. If
	   this flag is enabled then client_cert and client_key from the
	   client structure in the union tls (from tls_init_config_t)
	   needs to be passed to tls_session_init() */
	TLS_USE_CLIENT_CERT = 0x08,
#ifdef CONFIG_WPA2_ENTP
	TLS_WPA2_ENTP = 0x10,
#endif
	/* Set this bit if given client_cert (client mode) or server_cert
	   (server mode) is a chained buffer */
	TLS_CERT_BUFFER_CHAINED = 0x20,
} tls_flags_t;

typedef struct {
	/** OR of flags defined in \ref tls_flags_t */
	int flags;
	/** Either a client or a server can be configured at a time through
	   tls_session_init(). Fill up appropriate structure from the
	   below union depending on your requirement. */
	union {
		/** Structure for client TLS configuration */
		struct {
			/**
			 * Needed if the RADIUS server mandates verification of
			 * CA certificate. Otherwise set to NULL.*/
			const unsigned char *ca_cert;
			/** Size of CA_cert */
			int ca_cert_size;
			/**
			 * Needed if the server mandates verification of
			 * client certificate. Otherwise set to NULL. In
			 * the former case please OR the flag
			 * TLS_USE_CLIENT_CERT to flags variable in
			 * tls_init_config_t passed to tls_session_init()
			 */
			const unsigned char *client_cert;
			/** Size of client_cert */
			int client_cert_size;
			/** Client private key */
			const unsigned char *client_key;
			/** Size of client key */
			int client_key_size;
		} client;
		/** Structure for server TLS configuration */
		struct {
			/** Mandatory. Will be sent to the client */
			const unsigned char *server_cert;
			/** Size of server_cert */
			int server_cert_size;
			/**
			 * Server private key. Mandatory.
			 * For the perusal of the server
			 */
			const unsigned char *server_key;
			/** Size of server_key */
			int server_key_size;
			/**
			 * Needed if the server wants to verify client
			 * certificate. Otherwise set to NULL.
			 */
			const unsigned char *client_cert;
			/** Size of client_cert */
			int client_cert_size;
		}server;
	} tls;
} tls_init_config_t;

tls_handle_t tls_handle;
tls_init_config_t tls_cfg;

int tls_lib_init(void);
int tls_session_init(tls_handle_t *h, int sockfd,
		     const tls_init_config_t *cfg);
int tls_send(tls_handle_t h, const void *buf, int len);
int tls_recv(tls_handle_t h, void *buf, int max_len);
void tls_close(tls_handle_t *h);


char expired(Timer* timer)
{
	if (left_ms(timer) > 0)
		return 0;
	else
		return 1;
}


void countdown_ms(Timer* timer, unsigned int timeout)
{
	timer->timeout = timeout;
	timer->start_timestamp = os_ticks_get();
}


void countdown(Timer* timer, unsigned int timeout)
{
	/* converting timeout in milliseconds */
	timer->timeout = timeout * 1000;
	timer->start_timestamp = os_ticks_get();
}


int left_ms(Timer* timer)
{
	unsigned current_timestamp = os_ticks_get();
	int time_diff_ms = current_timestamp - timer->start_timestamp;
	return timer->timeout - time_diff_ms;
}


void InitTimer(Timer* timer)
{
	
}


int mbed_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	int val = 0;
	int recv_len = 0;
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(n->my_socket, &fds);

	setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout_ms, sizeof(timeout_ms));

	do {
		/* Will do SSL later
		if (tls_handle)
			val = tls_recv(tls_handle, buffer + recv_len, len - recv_len);
		*/
		val = read(n->my_socket, buffer + recv_len, len - recv_len);
		if (val < 1)
			break;
		recv_len += val;
	} while (recv_len < len);

	return recv_len;
}


int mbed_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	/* Will do SSL later	
	if (tls_handle)
		return tls_send(tls_handle, buffer, len);
	*/
	return write(n->my_socket, buffer, len);
}


static int create_socket(void) {
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	return sockfd;
}

static void close_socket(int *sockfd) {
	if (-1 == *sockfd)
		return;
	close(*sockfd);
	*sockfd = -1;
}

void mbed_disconnect(Network* n)
{
	close_socket(&n->my_socket);
}


void NewNetwork(Network* n)
{
	n->my_socket = 0;
	n->mqttread = mbed_read;
	n->mqttwrite = mbed_write;
	n->disconnect = mbed_disconnect;
	

	/* Will do SSL later */
	/* tls_lib_init(); */
}

int connect_socket(int socket_fd, char *addr, int port) {
	int rc = -1;
	int connect_status = -1;
	struct hostent *host;
	struct sockaddr_in dest_addr;

	host = gethostbyname(addr);

	if (NULL != host) {
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(port);
		dest_addr.sin_addr.s_addr = *(long*) (host->h_addr);
		memset(&(dest_addr.sin_zero), '\0', 8);

		connect_status = connect(socket_fd, (struct sockaddr *) &dest_addr,
				sizeof(struct sockaddr));
		if (-1 != connect_status) {
			rc = 0;
		}
	}
	return rc;
}

#define ROOT_CA_LOCATION "X"

int ConnectNetwork(Network* n, char* addr, int port)
{
	int rc = -1;
	n->my_socket = create_socket();
	if (n->my_socket != -1) {
		rc = connect_socket(n->my_socket, addr, port);
	}


	/*
	  Will do SSL later
	*/
	/*
	if (rc != 0) {
		return rc;
	}


	tls_cfg.flags = TLS_CHECK_SERVER_CERT;
	tls_cfg.tls.client.client_cert = NULL;
	tls_cfg.tls.client.client_cert_size = 0;
	tls_cfg.tls.client.ca_cert = (unsigned char *)ROOT_CA_LOCATION;
	tls_cfg.tls.client.ca_cert_size = strlen(ROOT_CA_LOCATION);

	rc = tls_session_init(&tls_handle, n->my_socket, &tls_cfg);
	if (rc != 0) {
		close_socket(&n->my_socket);
	}
	*/

	return rc;
}


