#include "huffman.h"
#include <stdlib.h>
#include <string.h>
typedef struct{
    unsigned int capacity;
	int top;
    HeapNode **array;
} Stack;

Stack *create_Stack(int capacity){
	Stack *new = malloc(sizeof(Stack));
	new->top = -1; // index of the top element
	new->capacity = capacity;
	new->array = malloc(sizeof(HeapNode*)*capacity);
	return new;
}

void push(Stack *stack, HeapNode *node){
	if(stack->top != stack->capacity-1){

		stack->array[stack->top+1] = node;
		stack->top += 1;
	}
}

HeapNode *pop(Stack *stack){
	if(stack->top != -1){
		HeapNode *result = stack->array[stack->top];
		stack->top -= 1;
		return result;
	}
	return NULL;
}


void make_freq_table(char *buf, HashTable *table, int nread){
    for(int i=0; i < nread; i++){
        add_frequency(table, &buf[i]);
    }
}

int isleaf(HeapNode* root){ 

	return (root->left) == NULL && (root->right) == NULL;
}


HeapNode *Minheap_to_huffmantree(MinHeap *heap, int num_symbols){ 
	HeapNode *left, *right, *top; 
	while (heap->size > 1) { 

		left = extractMin(heap); 
		right = extractMin(heap);
		printf("SIZE=%d\n", heap->size);
		top = create_HeapNode('*', (left->frequency)+(right->frequency)); 
		top->left = (struct HeapNode*)left;
		top->right = (struct HeapNode*)right; 
		top->num_nodes = left->num_nodes + right->num_nodes + 1;
		insert_heap(heap, top);

		printf("heap %d / %d\n", heap->size, heap->capacity);
	} 

	// Step 4: The remaining node is the 
	// root node and the tree is complete. 
	return extractMin(heap); 
} 

HashTable *create_lookup_table(HeapNode *tree, int tablesize){
	HashTable *table = create_HashTable(tablesize);
	char code[tablesize];
	memset(code, '\0', tablesize*sizeof(char));
	int index = 0;
	get_code(tree, table, code, index);
	return table;
}

void get_code(HeapNode *tree, HashTable *table, char *code, int index){


	if(tree->left != NULL){
		code[index] = '0';
		get_code((HeapNode*)tree->left, table, code, index+1);
	}

	if(tree->right != NULL){
		code[index] = '1';
		get_code((HeapNode*)tree->right, table, code, index+1);
	}

	if(isleaf(tree)){
		insert_table(table, tree->symbol, code, index);
	}

}

void print_Huffman(HeapNode *tree){
	if(tree != NULL){
		if(tree->left && tree->right){
			HeapNode *left = (HeapNode*)tree->left;
			HeapNode *right = (HeapNode*)tree->right;
			printf("%c: %d{left: %c, right: %c}\n", *tree->symbol, tree->frequency, *(left->symbol), *(right->symbol));	
		}
		else if(tree->left && !tree->right){
			HeapNode *left = (HeapNode*)tree->left;
			printf("%c: %d{left: %c, right: None}\n", *tree->symbol, tree->frequency, *left->symbol);	
		}
		else if(!tree->left && tree->right){
			HeapNode *right = (HeapNode*)tree->right;
			printf("%c: %d{left: None, right: %c}\n", *tree->symbol, tree->frequency, *right->symbol);	
		}
		else{
			printf("%c: %d \n", *tree->symbol, tree->frequency);
		}
		print_Huffman((HeapNode*)tree->left);
		print_Huffman((HeapNode*)tree->right);
	}
}

void postorder_Huffman(HeapNode *tree, unsigned char *header_tree_char, FILE *file, int *num_bits, int *total_bits){
	if(tree != NULL){
		postorder_Huffman((HeapNode*)tree->left, header_tree_char, file, num_bits, total_bits);
		postorder_Huffman((HeapNode*)tree->right, header_tree_char, file, num_bits, total_bits);
		
		int leaf = isleaf(tree);
		if(leaf == 0){
			printf("0");
		}
		else{
			printf("1%s", tree->symbol);
		}
		
		// encounter leaf
		if(isleaf(tree)){
			// encounter leaf write 1.
			*header_tree_char |= 1 << ( 7 - (*num_bits));
			*num_bits += 1;
			*total_bits += 1;
			if(*num_bits == 8){
				fwrite(header_tree_char, sizeof(char), 1, file);
				*header_tree_char = 0;
				*num_bits = 0;
			}
			// write bits of symbol after 1.
			for(int i = 7; i >= 0; i--){
				*header_tree_char |= ((*tree->symbol >> i) & 1)<<(7-*num_bits);
				*num_bits +=1;
				*total_bits += 1;
				if(*num_bits == 8){
					fwrite(header_tree_char, sizeof(char), 1, file);
					*header_tree_char = 0;
					*num_bits = 0;
				}
			}
		// encouter node.
		}else{
			// write 0, we don't need shift here, just increment num_bits
			// in this way, we start write at position [num_bits + 1]
			*num_bits += 1;
			*total_bits += 1;
			if(*num_bits == 8){
				fwrite(header_tree_char, sizeof(char), 1, file);
				*header_tree_char = 0;
				*num_bits = 0;
			}
		}
	}
}


HeapNode *decode_rebuild_HuffmanTree(FILE *file, int num_nodes){
	// Note: byte:xxxx xxxx
	//		index:7654 321
	Stack *stack = create_Stack(num_nodes);

	int total_nodes_counter = 0;
	int symbol_bits_counter = 0;
	int bits_need_in_next_byte = 0;
	int bit_index = -1; // use of right shift, 7 is the first bit from left.
	unsigned char code;
	unsigned char symbol;
	HeapNode *HuffmanTree = NULL;

	// 1. read one byte each time from file
	while(total_nodes_counter < num_nodes){

		if(bit_index < 0){
			fread(&code, sizeof(unsigned char), 1, file);
			bit_index = 7;
		}
		// 2. read each bit of every btye read
		// 3. if encounter 1, push 8 bits ASCII code followed into stack
		if((code >> bit_index) & 1){
			bits_need_in_next_byte = 8-bit_index; // caculate how many bits need from next byte 
			// set rest of bit from code to symbol
			for(int i=bit_index-1; i>=0; i--){ //bit_index-1 start from next bit.
				if((code >> i) & 1){
					// (*code1) >> i & 1, right shift the ith bit form right in code 1 to the rightest position
					// e.g. 0100 0000 >> 6 == 0000 0001
					//      0000 0001 & 1 == 0000 0001
					// << (7 - symbol_bits_counter), append the bit to the end of symbol char
					// e.g. symbol = 10xx xxxx, symbol_bits_couter == 2
					//      1<<(7-2) == 0010 0000
					//      10xx xxxx |= 0010 0000 == 101x xxxx, use or opreation to assign the value
					symbol |= ((code >> i) & 1) << (7 - symbol_bits_counter);
					symbol_bits_counter += 1; // increment counter
					if(symbol_bits_counter == 8){
						HeapNode *node = create_HeapNode(symbol, 1);
						push(stack, node);
						total_nodes_counter += 1;
						symbol = 0;
						symbol_bits_counter = 0;
					}
				}
				// if the next bit is 0, we just need to increment the counter to obtain the 0
				else{
					symbol_bits_counter += 1;
					if(symbol_bits_counter == 8){
						HeapNode *node = create_HeapNode(symbol, 1);
						push(stack, node);
						total_nodes_counter += 1;
						symbol = 0;
						symbol_bits_counter = 0;
					}
				}
			}
			bit_index = 7;
			 // code done
			// need one more byte:
			if(bits_need_in_next_byte > 0){
				// if the 
				fread(&code, sizeof(unsigned char), 1, file);
				for(bit_index = 7; bit_index > 7 - bits_need_in_next_byte; bit_index--){
					if((code >> bit_index) & 1){
						symbol |= ((code >> bit_index) & 1) << (7 - symbol_bits_counter);
						symbol_bits_counter += 1; // increment counter
						if(symbol_bits_counter == 8){
							HeapNode *node = create_HeapNode(symbol, 1);
							push(stack, node);
							total_nodes_counter += 1;
							symbol = 0;
							symbol_bits_counter = 0;
						}

					}
					else{
						symbol_bits_counter += 1;
						if(symbol_bits_counter == 8){
							HeapNode *node = create_HeapNode(symbol, 1);
							push(stack, node);
							total_nodes_counter += 1;
							symbol = 0;
							symbol_bits_counter = 0;
						}
					}
				}
			}
		}
		// if encounter 0, pop two element from stack and make HeapNode
		// which right child is the first symbol poped out, left child is the second pop
		else{
			bit_index -= 1;
			HeapNode *right = pop(stack);
			HeapNode *left = pop(stack);

			HeapNode *root = create_HeapNode('*', 1);
			root->left = (struct HeapNode *)left;
			root->right = (struct HeapNode *)right;
			// 4. push the HeapNode that we just created into stack.
			push(stack, root);
			total_nodes_counter += 1;
		}
	}
	HuffmanTree = pop(stack);
	return HuffmanTree;
}


void encode(FILE *fin, FILE *fout){
	// 1.read file into frequent table.
	char symbol[1];
	unsigned int size_file = 0;
	HashTable *table = create_HashTable(TABLESIZE);
	while(fread(symbol, sizeof(char), 1, fin) > 0){
		add_frequency(table, symbol);
		size_file += 1;
	}
	// 2. make huffman tree from freq table
	MinHeap *heap = Hashtable_to_MinHeap(table, TABLESIZE);
	HeapNode *tree = Minheap_to_huffmantree(heap, TABLESIZE);
	
	// 3. write number of node of the huffman tree, use 4-bytes unsigned integer
	unsigned int total_num_nodes = tree->num_nodes;
	fwrite(&total_num_nodes, sizeof(unsigned int), 1, fout);
	// 4. write how many bytes of original file, 4-bytes unsigned integer
	fwrite(&size_file, sizeof(unsigned int), 1, fout);
	// 5. write post-order of Huffman tree into file
	unsigned char header_char = 0;
	int num_bits = 0;
	int total_bits = 0;
	postorder_Huffman(tree, &header_char, fout, &num_bits, &total_bits);
	// set rest of the bits of the last byte to zero
	if(num_bits != 0){
		fwrite(&header_char, sizeof(char), 1, fout);
	}
	// 6. write the compressed file

	HashTable *lookup = create_lookup_table(tree, TABLESIZE);
	unsigned char code = 0;
	int bits_index = 7;
	fseek(fin, 0, SEEK_SET);
	while(fread(symbol, sizeof(char), 1, fin) > 0 ){
		HashNode *node = table_search(lookup, symbol);
		for(int i=0; node->code[i] != '\0'; i++){
			code |= ((node->code[i] == '1') ? 1 : 0) << bits_index;
			bits_index -= 1;
			if(bits_index == -1){
				fwrite(&code, sizeof(unsigned char), 1, fout);
				code = 0;
				bits_index = 7;
			}
		}
	}
	// 7. shift the rest bits to the zero
	if(bits_index != 7){
		fwrite(&code, sizeof(unsigned char), 1, fout);
	}
}



void decode(FILE *fin, FILE *fout){
	// 1. read the number of nodes, one unsinged int.
	unsigned int num_nodes = 0;
	fread(&num_nodes, sizeof(unsigned int), 1, fin);
	// 2. read the number of bytes of original file.
	unsigned int original_file_size = 0;
	fread(&original_file_size, sizeof(unsigned int), 1, fin);
	// 3. rebuild the tree.
	HeapNode *tree= decode_rebuild_HuffmanTree(fin, num_nodes);
	// 4. decode the compressed file.
	HashTable *table = create_lookup_table(tree, TABLESIZE);
	print_freq_table(table);
	printf("============================\n");
	HashTable *lookup = invert_table(table);
	print_freq_table(lookup);

	unsigned size_decode_file = 0;
	char code[TABLESIZE] = {'\0'};
	int code_index = 0;
	unsigned char byte;
	while(size_decode_file != original_file_size){
		fread(&byte, sizeof(unsigned char), 1, fin);
		for(int i=7; i>=0; i--){
			if((byte>>i) & 1){
				code[code_index] = '1';
			}
			else{
				code[code_index] = '0';
			}
			code_index += 1;
			HashNode *table_node = table_search_string(lookup, code);
			if(table_node != NULL){
				fwrite(table_node->code, sizeof(char), 1, fout);
				memset(code, '\0', TABLESIZE);
				code_index = 0;
				size_decode_file += 1;
				if(size_decode_file >= original_file_size){
					break;
				}
			}
		}
	}
}



// void decode_socket()