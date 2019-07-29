#include <netinet/in.h>    /* Internet domain header, for struct sockaddr_in */



int setup_server(struct sockaddr_in *addr, int num_queue, int port);

int connect_to_server(int port, const char *hostname);

int accept_connection(int listenfd);