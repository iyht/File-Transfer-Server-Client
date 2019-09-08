
#ifndef PORT
    #define PORT 666
#endif

#define NUM_QUEUE 5
#define BUFSIZE 1024

void write_to_socket(int fd, void *buf, int nread);

void get_filename(char *filename);

void get_fileinfo_user(int *size, char *filename, char *fileinfo, FILE *file);

void get_filenameinfo_socket(int fd, char *filename, char *fileinfo, int *size, int *recived_size);

int check_internet_newline(char *buf, int nbytes);
int ask_encode_file(FILE *file);