#include "hash.h"
typedef struct{
    char *symbol;
    int frequency;
    int num_nodes;
    struct HeapNode *left;
    struct HeapNode *right;
} HeapNode;

typedef struct{
    int size;
    int capacity;
    HeapNode **array;
 } MinHeap;

void print_Heap(MinHeap *heap);

void print_sorted(MinHeap *heap);

HeapNode *create_HeapNode(char symbol, int freq);

HashTable *create_HashTable(int tablesize);

MinHeap *create_MinHeap(int capacity);

MinHeap *Hashtable_to_MinHeap(HashTable *table, int num_symbols);

void build_min_heap(MinHeap *heap, int num_symbols);

void bubledown(MinHeap *heap, int index);

int find_min_index(MinHeap *heap, int index);

void bubleup(MinHeap *heap, int index);

int insert_heap(MinHeap *heap, HeapNode *new_node);

HeapNode *extractMin(MinHeap *heap);