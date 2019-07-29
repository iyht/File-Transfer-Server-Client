#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "hash.h"
#include <string.h>


HashNode *create_HashNode(char *symbol, unsigned int freq){
    HashNode *node = malloc(sizeof(HashNode));
    if(symbol != NULL){
        char *new_symbol = malloc(sizeof(char));
        *new_symbol = *symbol; 
        node->symbol = new_symbol;
    }    
    else{
        node->symbol = symbol;
    }

    node->frequency = freq;
    node->next = NULL;
    node->code = NULL;
    return node;
}

/*q
Create a hashtable and initial it.
 */
HashTable *create_HashTable(int tablesize){
    
    HashTable *table = malloc(sizeof(HashTable));
    table->size = 0;
    table->capacity = tablesize;
    table->array = malloc(sizeof(HashNode *)*tablesize);

    for(int i=0; i<tablesize; i++){
        table->array[i] = NULL;
    }
    return table;
}

/*
Hash function with The multiplication method
 */
int hash(HashTable *table, char *symbol){

    // % cannot use with double int
    return (int)floor(table->capacity * fmod((unsigned int)*symbol * 0.618 ,1));
}


int hash_string(HashTable *table, char *string){
    unsigned int total = 0;
    for(int i = 0; string[i] != '\0'; i++){
        total += string[i];
    }
    return (int)floor(table->capacity * fmod(total * 0.618 ,1));
}

/*
Return the pointer of the HashNode if the symbol is in the Hashtable,
return NULL otherwise
 */
HashNode *table_search(HashTable *table, char *symbol){
    int index = hash(table, symbol);
    if(*table->array[index]->symbol == *symbol){
        return table->array[index];
    }
    else{
        HashNode *next = (HashNode*)table->array[index]->next;
        while(next != NULL){
            if(*next->symbol == *symbol){
                return next;
            }
            next = (HashNode*)next->next;
        }
        return NULL;
    }

}


HashNode *table_search_string(HashTable *table, char *symbol){
    int index = hash_string(table, symbol);

    if(table->array[index] != NULL){
        if(strcmp(table->array[index]->symbol, symbol) == 0){
            return table->array[index];
        }
        else{
            HashNode *next = (HashNode*)table->array[index]->next;
            while(next != NULL){
                if(strcmp(next->symbol, symbol) == 0){
                    return next;
                }
                next = (HashNode*)next->next;
            }
            return NULL;
        }
    }else{
        return NULL;
    }

}


/*
increment frequency of corresbonding symbol
 */
void add_frequency(HashTable *table, char *symbol){
    // printf("now: %c\n", *symbol);
    int index = hash(table, symbol);
    // if find an empty spot
    // printf("hash: %d, symbol: %c\n", index, *symbol);
    if(table->array[index] == NULL){
        HashNode *new_node = create_HashNode(symbol, 1);
        table->array[index] = new_node;
        table->size += 1;
        // table[index].symbol = new_symbol;
        // table[index].frequency = 1;
    }
    else{
        // not empty, but the head of the chain is what we want.
        if(*(table->array[index]->symbol) == *symbol){
            table->array[index]->frequency += 1;
        }
        // not empty. the rest of the chain my
        // contains the symbel.
        else{
            HashNode *pre = table->array[index];
            HashNode *next = (HashNode *)table->array[index]->next;

            while(next != NULL && *(next->symbol) != *symbol){
                pre = next;
                next = (HashNode *)next->next;
            }
            // approach the end of chain. Create a new node.
            if(next == NULL){

                HashNode *new_node = create_HashNode(symbol, 1);
                pre->next = (struct HashNode*)new_node;
                table->size += 1;
            }else{
                next->frequency += 1;
            }
        }
    }


}

int print_freq_table(HashTable *table){
    int total_bytes = 0;
    // printf("%d\n", (int)(sizeof(table)/ sizeof(HashNode)) );
    // FILE *file = fopen("result.txt", "w");

    for(int i=0; i < table->capacity; i++){
        if(table->array[i] != NULL){
            // char msg[16];
            // snprintf(msg, 16,"(s: %s, f: %d)\n", table[i].symbol, table[i].frequency);
            // fwrite(msg, sizeof(char), strlen(msg), file);
            
            printf("(s: %s, f: %d, code: %s)\n", table->array[i]->symbol, table->array[i]->frequency, table->array[i]->code);
            // printf("%s\n", table->array[i]->code);

            // total_bytes += table[i].frequency;
            total_bytes += 1;
            // if there is chaining occures
            HashNode *next = (HashNode *)table->array[i]->next;
            while(next != NULL){
                printf("(s: %s, f: %d, code: %s)\n", next->symbol, next->frequency, next->code);
                next = (HashNode*)next->next;
                total_bytes += 1;
            }
            
        }
    }
    // fclose(file);
    return total_bytes;
}

/*
insert symbol into table with its huffuman code.
 */
void insert_table(HashTable *table, char *symbol, char *code, int position){
    // printf("now: %c\n", *symbol);
    int index = hash(table, symbol);
    // if find an empty spot
    if(table->array[index] == NULL){
        HashNode *new_node = create_HashNode(symbol, 1);
        
        char *new_code= malloc(sizeof(char)*(position+1));
        memset(new_code, '\0', position*sizeof(char));
        for (int i = 0; i < position; i++){
            new_code[i] = code[i];
        }
        new_code[position] = '\0';
        new_node->code = new_code;
        table->array[index] = new_node;
        
        //printf("%s\n", table->array[index]->code);
    }
    else{
        // not empty, but the head of the chain is what we want.
        if(*table->array[index]->symbol == *symbol){
            fprintf(stderr, "Symbol already there\n");
        }
        // not empty. the rest of the chain may
        // contains the symbel.
        else{
            HashNode *pre = table->array[index];
            HashNode *next = (HashNode *)table->array[index]->next;

            while(next != NULL && *(next->symbol) != *symbol){
                pre = next;
                next = (HashNode *)next->next;
            }
            // approach the end of chain. Create a new node.
            if(next == NULL){
                HashNode *new_node = create_HashNode(symbol, 1);
                
                char *new_code= malloc(sizeof(char)*(position+1));
                memset(new_code, '\0', position*sizeof(char));

                for (int i = 0; i < position; i++){
                    new_code[i] = code[i];
                }
                new_code[position] = '\0';
                new_node->code = new_code;
                pre->next = (struct HashNode *)new_node;
                // printf("%s\n", new_node->code);
            }else{
                fprintf(stderr, "Symbol already there\n");
            }
        }
    }

}


HashTable *invert_table(HashTable *table){
    HashTable *new_table = create_HashTable(table->capacity);
    for(int i=0; i<table->capacity; i++){
        if(table->array[i] != NULL){
            int index = hash_string(new_table, table->array[i]->code);
            if(new_table->array[index] == NULL){
                HashNode *new_node = create_HashNode(NULL, 1); // assign code to symbol
                new_node->symbol = table->array[i]->code;
                new_node->code = table->array[i]->symbol; // assign symbol to code
                new_table->array[index] = new_node;
            }
            else{
                HashNode *pre = new_table->array[index];
                HashNode *next = pre;
                while(next != NULL){
                    pre = next;
                    next = (HashNode *)next->next;
                }
                next = create_HashNode(NULL, 1);
                next->symbol = table->array[i]->code;
                next->code = table->array[i]->symbol;
                pre->next = (struct HashNode *)next;
            }
            // if there are some chaining nodes
            HashNode *chain_next = (HashNode *)table->array[i]->next;
            while(chain_next != NULL){
                index = hash_string(new_table, chain_next->code);
                if(new_table->array[index] == NULL){
                    HashNode *new_node = create_HashNode(NULL, 1); // assign code to symbol
                    new_node->symbol = chain_next->code;
                    new_node->code = chain_next->symbol; // assign symbol to code
                    new_table->array[index] = new_node;
                }else{
                    HashNode *pre = new_table->array[index];
                    HashNode *next = pre;
                    while(next != NULL){
                        pre = next;
                        next = (HashNode *)next ->next;
                    }
                    next = create_HashNode(NULL, 1);
                    next->symbol = chain_next->code;
                    next->code = chain_next->symbol;
                    pre->next = (struct HashNode *)next;
                }
                chain_next = (HashNode*)chain_next->next;
            }
        }

    }
    return new_table;
}





// int main(){
//     HashTable *table = create_HashTable(10);
//     add_frequency(table, "a");
//     add_frequency(table, "a");
//     add_frequency(table, "b");
//     add_frequency(table, "v");
//     add_frequency(table, "d");

//     add_frequency(table, "e");
//     add_frequency(table, "9");
//     add_frequency(table, "4");
//     add_frequency(table, "2");

//     add_frequency(table, "p");
//     add_frequency(table, "/");
//     add_frequency(table, "e");

//     print_freq_table(table);
//     printf("%d == %d\n", hash(table, "a"), hash(table, "a"));

// }