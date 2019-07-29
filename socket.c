#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>     /* inet_ntoa */
#include <netdb.h>         /* gethostname */
#include <sys/socket.h>

//server


// return listen fd waiting for client
int setup_server(struct sockaddr_in *addr, int num_queue, int port){

    //1. Initialize a server address associated with the given port.

    //Allow sockets across machines
    addr->sin_family = AF_INET;
    //The port the process will listen on
    addr->sin_port = htons(port);
    //Clear up memory by set to zero
    memset(&(addr->sin_zero), 0, 8);
    //Listen on all network interfaces.
    addr->sin_addr.s_addr = INADDR_ANY;

    //2. socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0){
        perror("socket");
        exit(1);
    }

    // Make sure we can reuse the port immediately after the
    // server terminates. Avoids the "address in use" error
    int on = 1;
    int status = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
        (const char *) &on, sizeof(on));
    if (status < 0) {
        perror("setsockopt");
        exit(1);
    }

    //3. bind
    if(bind(listenfd, (struct sockaddr *)addr, sizeof(*addr)) < 0){
        perror("bind");
        exit(1);
    }

    //4. listen
    if(listen(listenfd, num_queue < 0)){
        perror("listen");
        exit(1);
    }

    return listenfd;


}

int accept_connection(int listenfd){

    struct sockaddr_in new;
    unsigned int new_len = sizeof(new);
    fprintf(stderr, "Waiting...\n");
    int clientfd = accept(listenfd, (struct sockaddr *)&new, &new_len);
    if(clientfd < 0 ){
        perror("accept");
        return -1;
    }else{
        fprintf(stderr, "New connection from %s:%d\n",
            inet_ntoa(new.sin_addr), ntohs(new.sin_port));
        return clientfd;
    }

}












//Client



int connect_to_server(int port, const char *hostname) {
    //socket
    int soc = socket(PF_INET, SOCK_STREAM, 0);
    if (soc < 0) {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in addr;

    // Allow sockets across machines.
    addr.sin_family = PF_INET;
    // The port the server will be listening on.
    addr.sin_port = htons(port);
    // Clear this field; sin_zero is used for padding for the struct.
    memset(&(addr.sin_zero), 0, 8);

    // Lookup host IP address.
    struct hostent *hp = gethostbyname(hostname);
    if (hp == NULL) {
        fprintf(stderr, "unknown host %s\n", hostname);
        exit(1);
    }

    addr.sin_addr = *((struct in_addr *) hp->h_addr);

    // Request connection to server.
    if (connect(soc, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        exit(1);
    }

    return soc;
}