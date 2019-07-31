#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "socket.h"
#include "protocol.h"
#include "huffman.h"



#define NUM_QUEUE 5

int main(){
    
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    int listenfd = setup_server(addr, NUM_QUEUE, PORT);
    int fd = accept_connection(listenfd);

    while(1) {
        // char filename[BUFSIZE];
        printf("Waitting for File\n");

        char filename[BUFSIZE] = {'\0'};
        char fileinfo[BUFSIZE] = {'\0'};
        int size = 0;
        char new_filename[BUFSIZE];
        int recived_size = 0;
        get_filenameinfo_socket(fd, filename, fileinfo, &size, &recived_size);
        snprintf(new_filename, BUFSIZE, "server_%s", filename);
    
        FILE *file = fopen(new_filename, "wb+");

        if(file == NULL){
            fprintf(stderr, "Error creating file");
            exit(1);
        }


        char buf[BUFSIZE] = {0};
        int nbytes = 0;
        // memset(buf, '0', sizeof(buf));

        while((nbytes = recv(fd, buf, BUFSIZE, 0)) > 0){

            recived_size += nbytes;
            printf("%d bytes recived\n", nbytes);

            fwrite(buf, sizeof(char), nbytes, file);
            memset(buf, 0, BUFSIZE);
            // if the recived size equals to expected size, then end the loop.
            if(recived_size >= size){
                break;
            }


        }
        printf("Expected Size:%d bytes, Received Size: %d bytes\n", size, recived_size);
        char *first_char;
        printf("%s hhhhhhh\n", filename);
        if( (first_char = strstr(new_filename, ".huf")) != NULL){
            printf("hhhhhhh\n");
            *first_char = '\0';
            FILE *fout = fopen(new_filename, "wb");
            fseek(file, 0, 0);
            decode(file, fout);
            fclose(fout);
        }
        fclose(file);
    }
    close(fd);
    free(addr);
    close(listenfd);
    return 0;
}