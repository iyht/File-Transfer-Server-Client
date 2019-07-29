#include <stdlib.h>
#include <math.h>
#include "minheap.h"
#include <stdio.h>

HeapNode *create_HeapNode(char symbol, int freq){
    HeapNode *new = malloc(sizeof(HeapNode));
    char *new_symbol = malloc(sizeof(char));
    *new_symbol = symbol;
    new->symbol = new_symbol;
    new->num_nodes = 1;
    new->frequency = freq; 
    new->left = NULL;
    new->right = NULL;
    return new;
}

MinHeap *create_MinHeap(int capacity){
    MinHeap *new = malloc(sizeof(MinHeap));
    new->size = 0;
    new->capacity = capacity;
    new->array = malloc(sizeof(HeapNode*)*capacity);
    for(int i=0; i < capacity; i++){
        new->array[i] = NULL;
    }
    return new;
}

MinHeap *Hashtable_to_MinHeap(HashTable *table, int num_symbols){
    MinHeap *heap = create_MinHeap(num_symbols);
    int current_heap_index = 0;
    for(int i=0; i<table->capacity; i++){
        if(table->array[i] != NULL){
            heap->array[current_heap_index] = create_HeapNode(*table->array[i]->symbol, table->array[i]->frequency);
            current_heap_index += 1;
            heap->size += 1;
            HashNode *next = (HashNode*)table->array[i]->next;
            while(next != NULL){
                heap->array[current_heap_index] = create_HeapNode(*next->symbol, next->frequency);
                current_heap_index += 1;
                heap->size +=1;
                next = (HashNode*)next->next;
            }
            printf("%d / %d\n", heap->size, heap->capacity);
        }
    }

    build_min_heap(heap, heap->size);
    print_Heap(heap);

    return heap;
}



void build_min_heap(MinHeap *heap, int num_symbols){

    for(int i = floor(num_symbols-2/2); i >= 0; i--){
        bubledown(heap, i);
    }
}




void bubledown(MinHeap *heap, int index){
    int origin = index;
    int min_index = find_min_index(heap, origin);
    while(min_index != origin){
        HeapNode *temp= heap->array[origin];
        heap->array[origin] = heap->array[min_index];
        heap->array[min_index] = temp;
        origin = min_index;
        min_index = find_min_index(heap, origin);
    }
}


/*
Find the index amount of root, left and right node in heap with corresobonding index.
 */
int find_min_index(MinHeap *heap, int index){
    // int min_index;
    // There are left and right child
    if(2*index + 2 <= heap->size - 1){ 
        // Find the min amount three node;
        if(heap->array[index]->frequency <= heap->array[2*index + 1]->frequency && heap->array[index]->frequency <= heap->array[2*index+2]->frequency){
            return index;    
        }
        else if(heap->array[2*index+1]->frequency <= heap->array[index]->frequency && heap->array[2*index+1]->frequency <= heap->array[2*index+2]->frequency){
            return index*2+1;
        }
        else{
            return index*2+2;
        }
    }
    // There is only left child
    else if (2*index + 1 <= heap->size - 1){
        if(heap->array[index]->frequency <= heap->array[2*index+1]->frequency){
            return index;
        }
        else{
            return index*2+1;
        }
    }

    else{
        return index;
    }
}



void bubleup(MinHeap *heap, int index){
    int min_index = index;
    while(min_index > 0){
        if(heap->array[index]->frequency < heap->array[(int)floor(index-1/2)]->frequency){
            HeapNode *temp = heap->array[index];
            heap->array[index] = heap->array[(int)floor(index/2)];
            heap->array[(int)floor(index-1/2)] = temp;
            min_index = (int)floor(index-1/2);
        }
        else{
            break;
        }
    }
}


/*
Return 0 if we successful inserted new heapnode pointer.
Return 1 if heap is full;
 */
int insert_heap(MinHeap *heap, HeapNode *new_node){
    if(heap->size < heap->capacity){
        heap->array[heap->size] = new_node;
        heap->size += 1;
        bubleup(heap, heap->size-1);
        return 0;
    }
    return 1;
}


HeapNode *extractMin(MinHeap *heap){
    HeapNode *min = heap->array[0];
    heap->array[0] = heap->array[heap->size-1];
    heap->array[heap->size-1] = NULL;
    heap->size -= 1;
    bubledown(heap, 0);
    return min;
}


void print_Heap(MinHeap *heap){
    for(int i=0; i<heap->size; i++){
        printf("Heap: symbol=%c, freq=%d\n", *heap->array[i]->symbol, heap->array[i]->frequency);
    }

}


void print_sorted(MinHeap *heap){
    int size = heap->size;
    for(int i=0; i<size; i++){
        HeapNode *node = extractMin(heap);
        printf("Heap: size=%d, symbol=%c, freq=%d\n",heap->size, *node->symbol, node->frequency);
    }
}