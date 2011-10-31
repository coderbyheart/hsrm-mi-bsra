/* netcp.c */
/**
 * netcp - Programm zum Kopieren von Dateien über ein Netzwerk
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/stat.h>
#include "include/netcp.h"

/* Globales struct, dass die Client-Configuration enthält */
static options client_opts;
/* Socket zum Server */
static int server_socket;
static int fp;
static int debuglevel = DEFAULT_DEBUG_LEVEL;

/**
 * Gibt Anleitung aus.
 */
void client_usage() {
	printf("netcp {Optionen} <Quelle> {<Ziel>}\n");
	printf("Optionen:\n");
	printf(
			"\t-b <Puffergroesse>\tGröße des Kopierpuffers festlegen (default: %d)\n",
			DEFAULT_BUFFER_SIZE);
	printf("\t-p <Portnummer>\tTCP/IP-Portnummer angeben (default: %d)\n",
			DEFAULT_PORT);
	printf("\t-d <Level>\tDebug-Level, 0-3 (default: %d)\n",
			DEFAULT_DEBUG_LEVEL);
}

/**
 * Parst die übergebenen Argumente.
 */
void client_parse_params(int argc, char *argv[]) {
	int param = 0;
	client_opts.port = DEFAULT_PORT;
	client_opts.buffer = DEFAULT_BUFFER_SIZE;

	while ((param = getopt(argc, argv, "h?b:p:d:")) != -1) {
		switch (param) {
		case 'b':
			client_opts.buffer = atoi(optarg);
			if (client_opts.buffer > MAX_BUFFER_SIZE) {
				printf("[Error] Max buffer size is %d\n", MAX_BUFFER_SIZE);
				exit(18);
			}
			break;
		case 'p':
			client_opts.port = atoi(optarg);
			break;
		case 'd':
			debuglevel = atoi(optarg);
			break;
		case 'h':
		case '?':
		default:
			client_usage();
			exit(1);
		}
	}

	if (argc - optind > 2 || argc - optind < 1) {
		client_usage();
		exit(2);
	}

	char *in = argv[optind];
	char *out = argv[++optind];
	char *colon;
	struct hostent *he;
	struct in_addr a;

	if ((colon = strchr(in, ':'))) {
		if (strchr(out, ':')) {
			client_usage();
			exit(3);
		}
		*colon = '\0';

		he = gethostbyname(in);
		if (he) {

			while (*he->h_addr_list) {
				bcopy(*he->h_addr_list++, (char *) &a, sizeof(a));

				in = inet_ntoa(a);
			}
		} else
			herror(in);

		if (strcmp(in, "localhost") == 0) {
			client_opts.destination = "127.0.0.1";
		} else {
			client_opts.destination = in;
		}

		client_opts.infile = colon + 1;
		if (in == '\0') {
			client_usage();
			exit(4);
		}
		client_opts.direction = FROM_REMOTE;
	} else {
		client_opts.infile = in;
		client_opts.direction = DIR_ERROR;
	}

	if ((colon = strchr(out, ':'))) {
		if (strchr(in, ':')) {
			client_usage();
			exit(5);
		}
		*colon = '\0';

		he = gethostbyname(out);
		if (he) {

			while (*he->h_addr_list) {
				bcopy(*he->h_addr_list++, (char *) &a, sizeof(a));

				out = inet_ntoa(a);
			}
		} else
			herror(out);

		if (strcmp(out, "localhost") == 0) {
			client_opts.destination = "127.0.0.1";
		} else {
			client_opts.destination = out;
		}
		client_opts.outfile = colon + 1;
		if (out == '\0') {
			client_usage();
			exit(6);
		}
		client_opts.direction = TO_REMOTE;
	} else {
		client_opts.outfile = out;
	}

	if (client_opts.direction == DIR_ERROR) {
		client_usage();
		exit(7);
	}
}

/* Zeigt die aktuelle Konfiguration an */
void client_printopts() {
	printf("In:\t%s\n", client_opts.infile);
	printf("Out:\t%s\n", client_opts.outfile);
	printf("Host:\t%s\n", client_opts.destination);
	printf("Dir:\t%d\n", client_opts.direction);
	printf("Buffer:\t%lu\n", client_opts.buffer);
	printf("Port:\t%lu\n", client_opts.port);
}

/* Verbindung zum Server herstellen */
void client_connect() {
	if (debuglevel > 0)
		printf("Connecting to %s:%lu\n", client_opts.destination,
				client_opts.port);
	struct sockaddr_in servaddr;
	char *host = client_opts.destination;

	if (inet_aton(host, &servaddr.sin_addr) == 0) {
		perror("inet_aton");
		if (debuglevel > 1)
			printf("Host %s\n", host);
		exit(8);
	};
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(client_opts.port);
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(9);
	}
	if (connect(server_socket, (struct sockaddr *) &servaddr, sizeof(servaddr))
			< 0) {
		perror("connect");
		exit(10);
	}
}

/* Sendet eine Message zum Server */
void client_send_message(netcp_message *m, int readResponse) {
	int writeBytes, readBytes;
	switch (m->type) {
	case SET_BUFFER:
		if (debuglevel > 1)
			printf("< Buffersize: %lu\n", m->data.buffersize);
		break;
	case RECV_OK:
		if (debuglevel > 1)
			printf("< OK\n");
		break;
	case TERMINATE:
		if (debuglevel > 1)
			printf("< Terminate\n");
		break;
	case FILENAME:
		if (debuglevel > 1)
			printf("< Filename: %s\n", m->data.filename);
		break;
	case BEGIN_DATA:
		if (debuglevel > 1)
			printf("< Begin data\n");
		break;
	case END_DATA:
		if (debuglevel > 1)
			printf("< End data\n");
		break;
	case CLOSE:
		if (debuglevel > 1)
			printf("< close\n");
		break;
	default:
		break;
	}
	/* Send message */
	if ((writeBytes = write(server_socket, m, sizeof(netcp_message))) < 0) {
		perror("send_message");
		printf("Failed to send message\n");
		exit(13);
	}
	if (!readResponse)
		return;
	/* read response */
	netcp_message *responseMessage = malloc(sizeof(netcp_message));
	if ((readBytes
			= read(server_socket, responseMessage, sizeof(netcp_message))) < 0) {
		perror("read_response");
		printf("Failed to read response\n");
		exit(14);
	}
	if (responseMessage->type != RECV_OK) {
		if (debuglevel > 1)
			printf("[ERROR]\n");
		free(responseMessage);
		exit(11);
	} else {
		free(responseMessage);
	}
}

/* Verbindung zum Server beenden */
void client_disconnect() {
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = CLOSE;
	client_send_message(m, 0);
	free(m);
	close(server_socket);
	if (debuglevel > 1)
		printf("Connection closed.\n");
}

/* Setzt die Puffergröße */
void client_set_buffersize() {
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = SET_BUFFER;
	m->data.buffersize = client_opts.buffer;
	client_send_message(m, 1);
	free(m);
}

/* Dateiname zum Server senden */
void client_send_filename() {
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = FILENAME;
	strcpy(m->data.filename, client_opts.outfile);
	client_send_message(m, 1);
	free(m);
}

/* Dateiname der Quelle zum Server senden */
void client_send_srcname() {
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = FILENAME;
	strcpy(m->data.filename, client_opts.infile);
	client_send_message(m, 1);
	free(m);
}

/* Beginne mit Datenübertragung */
void client_send_data_begin() {
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = BEGIN_DATA;
	client_send_message(m, 1);
	free(m);
}

/* Beende Datenübertragung */
void client_send_data_end() {
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = END_DATA;
	client_send_message(m, 1);
	free(m);
}

/* Datei zum Server senden */
void client_copy_to_server() {
	/* Lokale Datei öffnen */
	int fp;
	if ((fp = open(client_opts.infile, O_RDONLY)) < 0) {
		perror("local file");
		exit(12);
	}
	/* Dateiname senden */
	client_send_filename();
	/* Beginne mit Datentransfer */
	client_send_data_begin();

	/* Jeder Datentransfer beginnt mit einer DATA-Message */
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = DATA;
	char *bytes = malloc(client_opts.buffer);
	int writeBytes, readBytes;
	while (read(fp, bytes, client_opts.buffer)) {
		/* Send data message */
		if ((writeBytes = write(server_socket, m, sizeof(netcp_message))) < 0) {
			perror("send_data_message");
			printf("Failed to send data message\n");
			exit(18);
		}
		/* Send actual data */
		if (debuglevel > 2)
			printf("<< %s\n", bytes);
		if ((writeBytes = write(server_socket, bytes, client_opts.buffer)) < 0) {
			perror("send_data");
			printf("Failed to send data\n");
			exit(15);
		}
		bzero(bytes, client_opts.buffer);
		/* read response */
		netcp_message *responseMessage = malloc(sizeof(netcp_message));
		if ((readBytes = read(server_socket, responseMessage,
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
	client_send_data_end();
}

/* Dateiname zum Server senden */
void server_open() {
	client_send_srcname();
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = OPEN;
	client_send_message(m, 1);
	free(m);
}

/* Zieldatei öffnen */
int open_file(char *filename) {
	if ((fp = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP
			| S_IWGRP)) < 0)
		return 0;
	return 1;
}

/* Daten vom Server empfangen */
void server_read() {
	int readBytes;
	void *buffer = malloc(client_opts.buffer);
	if (debuglevel > 1)
		printf("Reading data %lu\n", client_opts.buffer);
	readBytes = read(server_socket, buffer, client_opts.buffer);
	if (debuglevel > 2)
		printf("(%d bytes) >> %s", readBytes, (char *) buffer);
	write(fp, buffer, client_opts.buffer);
	free(buffer);
}

/* Empfang quittieren */
void client_send_ok() {
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = RECV_OK;
	client_send_message(m, 0);
	free(m);
}

/* Eingehende Nachricht vom Server verarbeiten */
int client_handle_message(netcp_message *m) {
	switch (m->type) {
	case SET_BUFFER:
		if (debuglevel > 1)
			printf("> Buffersize: %lu\n", m->data.buffersize);
		client_opts.buffer = m->data.buffersize;
		client_send_ok();
		break;
	case BEGIN_DATA:
		if (debuglevel > 1)
			printf("> Begin data\n");
		client_send_ok();
		break;
	case END_DATA:
		if (debuglevel > 1)
			printf("> End data\n");
		client_send_ok();
		return 0;
	case DATA:
		if (debuglevel > 1)
			printf("> data\n");
		server_read();
		client_send_ok();
		break;
	default:
		client_send_ok();
		break;
	}
	return 1;
}

/* Datei vom Server empfangen */
void server_copy_to_client() {
	server_open();

	netcp_message *m = malloc(sizeof(netcp_message));
	netcp_message *response = malloc(sizeof(netcp_message));
	m->type = GET_BUFFER;
	client_send_message(m, 0);

	read(server_socket, response, sizeof(netcp_message));
	client_handle_message(response);
	open_file(client_opts.outfile);

	m->type = GET;
	client_send_message(m, 0);

	int i = 1;
	while (i == 1) {
		read(server_socket, response, sizeof(netcp_message));
		if (!client_handle_message(response)) {
			i--;
		}

	}
	close(fp);
	free(response);
	free(m);
}

/* Verbinding zum Server beenden */
void server_close() {
	netcp_message *m = malloc(sizeof(netcp_message));
	m->type = CLOSE;
	client_send_message(m, 1);
	free(m);
}

int main(int argc, char *argv[]) {
	client_parse_params(argc, argv);
	if (debuglevel > 1)
		client_printopts();

	client_connect();
	client_set_buffersize();

	if (client_opts.direction == TO_REMOTE) {
		client_copy_to_server();
	} else if (client_opts.direction == FROM_REMOTE) {
		server_copy_to_client();
	}
	client_disconnect();

	return 0;
}
