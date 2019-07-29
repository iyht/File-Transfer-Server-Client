
#include "minheap.h"
#include <stdio.h>



int isleaf(HeapNode* root);
void make_freq_table(char *buf, HashTable *table, int nread);
HeapNode *Minheap_to_huffmantree(MinHeap *heap, int num_symbols);
HashTable *create_lookup_table(HeapNode *tree, int tablesize);
void get_code(HeapNode *tree, HashTable *table, char *code, int index);
void print_Huffman(HeapNode *tree);
void postorder_Huffman(HeapNode *tree, unsigned char *header_tree_char, FILE *file, int *num_bits, int *total_bytes);
HeapNode *decode_rebuild_HuffmanTree(FILE *file, int num_char);
void encode(FILE *fin, FILE *fout);
void decode(FILE *fin, FILE *fout);