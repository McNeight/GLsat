/* Prototypes for the communications functions */

#include "comms.c"

int connectsock(char *host, char *service, char *protocol);
int send_command(int sock, char *command);
int get_response(int sock, char *buf);
gint timeout_callback( gpointer data );
