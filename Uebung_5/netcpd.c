/* netcpd.c */
/**
 * netcpd - Server für netcp
 */
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "include/netcp.h"
#include <signal.h>

static int port = DEFAULT_PORT;
static int server_socket, client_socket = 0;
static int buffersize = DEFAULT_BUFFER_SIZE;
static int fp;
static int debuglevel = DEFAULT_DEBUG_LEVEL;

/**
 * Gibt Anleitung aus.
 */
void server_usage() {
	printf("netcpd {Optionen}\n");
	printf("Optionen:\n");
	printf("\t-p <Portnummer>\tTCP/IP-Portnummer angeben (default: %d)\n", port);
	printf("\t-d <Level>\tDebug-Level, 0-3 (default: %d)\n",
			DEFAULT_DEBUG_LEVEL);
}

/**
 * Parst die übergebenen Argumente.
 */
void server_parse_params(int argc, char *argv[]) {
	int param = 0;
	while ((param = getopt(argc, argv, "h?p:d:")) != -1) {
		switch (param) {
		case 'p':
			port = atoi(optarg);
			break;
		case 'd':
			debuglevel = atoi(optarg);
			break;
		case 'h':
		case '?':
		default:
			server_usage();
			exit(1);
		}
	}
}

/* Gibt die Konfiguration aus */
void server_printopts() {
	if (debuglevel > 0)
		printf("Starte Server an Port %d\n", port);
}

/* Sendet eine message zum Client */
void server_send_message(netcp_message *m) {
	/* Send message */
	write(client_socket, m, sizeof(netcp_message));
}

/* Sendet eine OK-Message zum Client */
void server_send_ok() {
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = RECV_OK;
	server_send_message(m);
	free(m);
}

/* Sendet eine Error-Message zum Client */
void server_send_error() {
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = RECV_ERROR;
	server_send_message(m);
	free(m);
}

/* Server beenden */
void server_terminate() {
	if (debuglevel > 0)
		printf("Termination.\n");
	close(server_socket);
}

/* Verbindung zum Client beenden */
void server_close() {
	close(client_socket);
	client_socket = 0;
}

/* Daten vom Client lesen */
void server_read_data() {
	int readBytes;
	void *buffer = malloc(buffersize);
	if (debuglevel > 2)
		printf("Reading data %d\n", buffersize);
	readBytes = read(client_socket, buffer, buffersize);
	if (debuglevel > 2)
		printf("(%d bytes) >> %s", readBytes, (char *) buffer);
	write(fp, buffer, buffersize);
	free(buffer);
}

/* Zieldatei öffnen */
int server_open_target(char *filename) {
	if ((fp = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP
			| S_IWGRP)) < 0)
		return 0;
	return 1;
}

/* Zieldatei schließen */
void server_close_target() {
	close(fp);
}

/* Daten zum Client senden beenden */
void server_send_data_end() {
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = END_DATA;
	server_send_message(m);
	free(m);
}

/* Sendet eine Datei zum Client */
void server_copy_to_client() {

	/* Jeder Datentransfer beginnt mit einer DATA-Message */
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = DATA;
	char *bytes = malloc(buffersize);
	int writeBytes, readBytes;
	while (read(fp, bytes, buffersize)) {
		/* Send data message */
		if ((writeBytes = write(client_socket, m, sizeof(netcp_message))) < 0) {
			perror("send_data_message");
			printf("Failed to send data message\n");
			exit(18);
		}
		/* Send actual data */
		if (debuglevel > 2)
			printf("<< %s\n", bytes);
		if ((writeBytes = write(client_socket, bytes, buffersize)) < 0) {
			perror("send_data");
			printf("Failed to send data\n");
			exit(15);
		}
		bzero(bytes, buffersize);
		/* read response */
		netcp_message *responseMessage = malloc(sizeof(netcp_message));
		if ((readBytes = read(client_socket, responseMessage,
				sizeof(netcp_message))) < 0) {
			perror("read_response");
			printf("Failed to read response\n");
			exit(16);
		}
		if (responseMessage->type != RECV_OK) {
			if (debuglevel > 1)
				printf("[ERROR]\n");
			free(responseMessage);
			exit(17);
		} else {
			free(responseMessage);
		}
	}
	free(m);
	close(fp);
	/* Beende Datentransfer */
	server_send_data_end();
}

/* Sendet Buffergröße zum Client */
void server_send_buffer() {
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = SET_BUFFER;
	m->data.buffersize = buffersize;
	server_send_message(m);
	free(m);
}

/* Eingehende Nachricht vom Client verarbeiten */
void server_handle_message(netcp_message *m) {
	switch (m->type) {
	case SET_BUFFER:
		if (debuglevel > 1)
			printf("> Buffersize: %lu\n", m->data.buffersize);
		buffersize = m->data.buffersize;
		server_send_ok();
		break;
	case TERMINATE:
		if (debuglevel > 1)
			printf("> Terminate\n");
		server_send_ok();
		server_terminate();
		break;
	case CLOSE:
		if (debuglevel > 1)
			printf("> Close\n");
		server_close();
		break;
	case FILENAME:
		if (debuglevel > 1)
			printf("> Filename: %s\n", m->data.filename);
		if (server_open_target(m->data.filename)) {
			server_send_ok();
		} else {
			server_send_error();
		}
		break;
	case BEGIN_DATA:
		if (debuglevel > 1)
			printf("> Begin data\n");
		server_send_ok();
		break;
	case END_DATA:
		if (debuglevel > 1)
			printf("> End data\n");
		server_close_target();
		server_send_ok();
		break;
	case DATA:
		if (debuglevel > 2)
			printf("> data\n");
		server_read_data();
		server_send_ok();
		break;
	case GET:
		if (debuglevel > 1)
			printf("> copy to client\n");
		server_copy_to_client();
		break;
	case GET_BUFFER:
		if (debuglevel > 1)
			printf("> send_buffer\n");
		server_send_buffer();
		break;
	default:
		server_send_ok();
		break;
	}
}

/* Startet den Server */
void server_start() {
	unsigned int clientlen;
	int readBytes, childPid;
	struct sockaddr_in servaddr, clientaddr;

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(2);
	}
	if (bind(server_socket, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
		exit(3);
	}
	if (listen(server_socket, 5) < 0) {
		perror("listen");
		exit(4);
	}
	clientlen = sizeof(clientaddr);
	for (;;) {

		/* Get new client socket conection */
		if ((client_socket = accept(server_socket,
				(struct sockaddr *) &clientaddr, &clientlen)) < 0) {
			perror("accept");
			exit(5);
		}

		/* Fork a child to handle the request */
		if ((childPid = fork()) < 0) {
			perror("fork");
			exit(6);
		} else if (childPid == 0) {
			if (debuglevel > 0)
				printf("Child: Client connected from host %s:%d\n", inet_ntoa(
						clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			while (client_socket > 0) {
				/* read request */
				netcp_message *requestMessage = malloc(sizeof(netcp_message));
				readBytes = read(client_socket, requestMessage,
						sizeof(netcp_message));
				if (readBytes < 0) {
					perror("read");
					server_close();
					exit(7);
				}
				server_handle_message(requestMessage);
				free(requestMessage);
			}
			if (debuglevel > 0)
				printf("Client disconnected from host %s:%d\n", inet_ntoa(
						clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			exit(0);
		} else {
			if (debuglevel > 0)
				printf("Forking child %d for client %s:%d\n", childPid,
						inet_ntoa(clientaddr.sin_addr), ntohs(
								clientaddr.sin_port));
			server_close();
		}
	}
}

/* Ctrl+C abfangen */
void server_setup_signals() {
	(void) signal(SIGINT, server_terminate);
}

int main(int argc, char *argv[]) {
	server_setup_signals();
	server_parse_params(argc, argv);
	server_printopts();
	server_start();
	return 0;
}
