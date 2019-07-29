#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <errno.h>

#include "socket.h"
#include "protocol.h"
/*
write buffer patch to fd
 */
void write_to_socket(int fd, char *buf, int nread){
    int sendb = send(fd, buf, nread, 0);
    if(sendb == -1){
        perror("send\n");
        exit(1);
    }
    printf("send %d\n", sendb);
    memset(buf, 0, BUFSIZ);
}

/*
get file name from user
 */
void get_filename(char *filename){
    int namesize = 0;
    printf("Enter file name:\n");

    while(((namesize = read(STDIN_FILENO, filename, BUFSIZE)) <= 0)){
        if(namesize < 0){
            perror("read file name\n");
            exit(1);
        }
        else{
            printf("Enter file name:\n");
        }
  
    }
    filename[namesize-1] = '\0';



}

/*
 */
void get_filenameinfo_socket(int fd, char *filename, char *fileinfo, int *size){
    int fileinfo_size = recv(fd, fileinfo, BUFSIZE, 0);
    if(fileinfo_size == -1){
        perror("recv: when getting fileinfo.");
        exit(1);
    }
    char *rest = fileinfo;
    strcpy(filename, strtok_r(rest, ",", &rest));
    *size = strtol(rest, NULL, 10);
}

/*
assign *filename in form of "file,size\0"
return 0 on success, 1 otherwise
 */
void get_fileinfo_user(int *size, char *filename, char *fileinfo, FILE *file){
    //get file descripter
    int fd = fileno(file);
    if(fd == -1){
        perror("fileno");
        exit(1);
    }
    //stat struct contian file info, but we need file descripter instead of FILE
    struct stat sb;
    //fail to call fstat
    if(fstat(fd, &sb) == -1){
        fprintf(stderr, "Can not get infomation of the file\n");
        exit(1);
    }
    //sucess, append size to fileinfo
    *size = sb.st_size;
    if(snprintf(fileinfo, BUFSIZE, "%s,%lld", filename, sb.st_size) < 0){
        exit(1);
    }
}

/*
check if server receving the end of the buffer.
return 1 client is done, 0 otherwise
 */
int check_internet_newline(char *buf, int nbytes){

    char internet_newline[BUFSIZE] = "end\r\n";
    if(strstr(buf, internet_newline) == NULL){
        return 1;
    }
    else{
        printf("hiiiiii\n");
        buf[nbytes-5] = '\0';
        return 0;
    }
    
}