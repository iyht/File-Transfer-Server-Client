typedef struct{
    char *symbol;
    unsigned int frequency;
    char *code;
    struct HashNode *next;
} HashNode;

typedef struct{
    int size;
    int capacity;
    HashNode **array;
} HashTable;

#define TABLESIZE 256

int hash(HashTable *table, char *symble);

HashTable *create_HashTable(int tablesize);


HashNode *table_search(HashTable *table, char *symbol);

int print_freq_table(HashTable *table);

void insert_table(HashTable *table, char *symbol, char *code, int position);

void add_frequency(HashTable *table, char *symbol);

HashTable *invert_table(HashTable *table);
int hash_string(HashTable *table, char *string);
HashNode *table_search_string(HashTable *table, char *symbol);