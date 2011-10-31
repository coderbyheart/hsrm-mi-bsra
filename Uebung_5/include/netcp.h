/* netcp.h */
#ifndef __NETCP_H__
/* Default port for the server */
#define DEFAULT_PORT 12345
/* Default buffer size */
#define DEFAULT_BUFFER_SIZE 512
/* Max buffer size for TCP is 16KB */
#define MAX_BUFFER_SIZE 16384
#define DEFAULT_DEBUG_LEVEL 0
#include <sys/param.h>
/* Message types */
enum netcp_message_type{NETCP_MESSAGE_RESERVED, SET_BUFFER, RECV_OK, RECV_ERROR, TERMINATE, CLOSE, FILENAME, BEGIN_DATA, DATA, END_DATA, OPEN, READ, GET, GET_BUFFER};
/**
 * Struktur um die Messages des Programms zu halten
 */
typedef struct _netcp_message netcp_message;
struct _netcp_message {
	enum netcp_message_type type;
	union {
		unsigned long int buffersize;
		char filename[PATH_MAX];
	} data;
	int sender;
};
/**
 * Struktur um die Optionen des Programms zu halten
 */
enum netcp_direction{NETCP_DIRECTION_RESERVED, TO_REMOTE, FROM_REMOTE, DIR_ERROR};
typedef struct _options options;
struct _options {
	char *infile, *outfile, *destination;
	enum netcp_direction direction;
	unsigned long int buffer;
	unsigned long int port;
};
#endif 
