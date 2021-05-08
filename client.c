#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>


#include "huffman.h"

#include "socket.h"
#include "protocol.h"


int main(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr, "Usage ./client hostname PORT\n");
        exit(1);
    }
    //Connect to server *need error checking later*
    int soc = connect_to_server(atoi(argv[2]),argv[1]);

    while(1){
        //Get a filename from user
        char filename[BUFSIZE/2];
        int size = 0;
        char fileinfo[BUFSIZE];
        int total_read = 0;

        get_filename(filename);

        FILE *file = fopen(filename, "rb");
        while(file == NULL){
            printf("File \"%s\" not found, please try again:\n", filename);
            get_filename(filename);
            file = fopen(filename, "rb");
        }



        if( ask_encode_file(file) == 0){
            char encode_file_name[BUFSIZE];
            snprintf(encode_file_name, BUFSIZE, "%s.huf", filename);
            FILE *encode_file = fopen(encode_file_name, "wb+");
            encode(file,encode_file);
            fseek(encode_file, 0, 0);


            get_fileinfo_user(&size, encode_file_name, fileinfo, encode_file);
            write_to_socket(soc, fileinfo, strlen(fileinfo));

            char buf[BUFSIZE] = {'\0'};
            int nread = 0;
            while((nread = fread(buf, sizeof(char), BUFSIZE, encode_file)) > 0){
                total_read +=nread;
                write_to_socket(soc, buf, nread);
            }
            fclose(encode_file);
        }

        else{
            // Read file into buffer. 
            get_fileinfo_user(&size, filename, fileinfo, file);
            write_to_socket(soc, fileinfo, strlen(fileinfo));
            char buf[BUFSIZE] = {'\0'};
            int nread = 0;
            while((nread = fread(buf, sizeof(char), BUFSIZE, file)) > 0){
                total_read +=nread;
                write_to_socket(soc, buf, nread);
            }
        }

        printf("total send %d", total_read);

        fclose(file);

        // Read file into buffer. 
        // char buf[BUFSIZE] = {'\0'};
        // int nread = 0;
        // int total_read = 0;
        // // HashTable *table = create_HashTable(TABLESIZE);
        // while((nread = fread(buf, sizeof(char), BUFSIZE, file)) > 0){
        //     // make_freq_table(buf, table, nread);
        //     // total_read +=nread;
        //     // write_to_socket(soc, buf, nread);
        // }



        // char fout_name[BUFSIZE];
        // snprintf(fout_name, BUFSIZE, "%s.huf", filename);
        // FILE *fout = fopen(fout_name, "wb");
        // encode(file,fout);
        
        // fclose(fout);
        

        // FILE *de_fin = fopen(fout_name, "rb");
        // FILE *de_fout = fopen("book_result.txt", "wb");
        // decode(de_fin, de_fout);
        // fclose(de_fin);
        // fclose(de_fout);

        //Read file into buffer. 
        // char buf[BUFSIZE] = {0};
        // int nread = 0;
        // int total_read = 0;
        // // HashTable *table = create_HashTable(TABLESIZE);
        // while((nread = fread(buf, sizeof(char), BUFSIZE, file)) > 0){
        //     // make_freq_table(buf, table, nread);
        //     // total_read +=nread;
        //     // write_to_socket(soc, buf, nread);
        // }

        // printf("total = %d", total_read);
        // printf("table: %d / %d \n", table->size, table->capacity);
        // printf("number of bytes in table: %d\n",print_freq_table(table));
        // MinHeap *heap = Hashtable_to_MinHeap(table, TABLESIZE);
        // print_sorted(heap);
        // /HeapNode *tree = Minheap_to_huffmantree(heap, TABLESIZE);
        
        // decode tree
        // int num_nodes = tree->num_nodes;
        // FILE *fout = fopen("out.huf", "wb");
        // unsigned char header_tree_char = 0;
        // int num_bits = 0;
        // int total_bits = 0; // total bits write into header about huffman tree.
        // postorder_Huffman(tree, &header_tree_char, fout, &num_bits, &total_bits);
        // if(num_bits != 0){
        //     fwrite(&header_tree_char, sizeof(char), 1, fout);
        // }
        // fclose(fout);
        
        // FILE *coded = fopen("out.huf", "rb");
        // HeapNode *decode = decode_rebuild_HuffmanTree(fout, total_bits, num_nodes);

        // print_Huffman(decode);
        // printf("===========================\n");
        // fclose(coded);

        // print_Huffman(tree);
        // HashTable *lookup = create_lookup_table(tree, TABLESIZE);
        // print_freq_table(lookup);
        // printf("number of bytes in out.huf: %d\n",total_bits);


        // char test = '[';
        // char *lookup_code = lookup->array[hash(lookup, &test)]->code;
        // // printf("size: %lu\n", sizeof(lookup_code));
        // for(int i=0 ; lookup_code[i] != '\0'; i++){
        //     printf("%c", lookup_code[i]);
        // }
        // printf("[: %s", (table_search(lookup, &test))->code);        

        // fclose(fout);
    }
    


    

    close(soc);
    return 0;

}