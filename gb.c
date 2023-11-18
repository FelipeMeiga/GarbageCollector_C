#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Block {
    size_t size;
    struct Block* next;
    int free;
    int mark;
} Block;

#define BLOCK_SIZE sizeof(Block)

Block* freeList = NULL; // List of free blocks
Block* usedList = NULL; // List of blocks in use

void* GC_malloc(size_t size) {
    Block* current = freeList;
    Block* prev = NULL;

    while (current != NULL) {
        if (current->free && current->size >= size) {
            current->free = 0;
            return (current + 1);
        }
        prev = current;
        current = current->next;
    }

    size_t totalSize = size + BLOCK_SIZE;
    Block* block = sbrk(totalSize);
    block->size = size;
    block->free = 0;
    block->mark = 0;
    block->next = usedList;
    usedList = block;
    return (block + 1);
}

void gc_collect() {
    Block *current = usedList;
    Block *prev = NULL;

    while (current != NULL) {
        if (current->mark == 0) {
            Block *unmarked = current;

            if (prev != NULL) {
                prev->next = current->next;
            } else {
                usedList = current->next;
            }

            unmarked->next = freeList;
            freeList = unmarked;
            unmarked->free = 1;

            current = (prev != NULL) ? prev->next : usedList;
        } else {
            current->mark = 0;
            prev = current;
            current = current->next;
        }
    }
}

int main() {
    printf("Testing garbage collector.\n");

    int* num1 = (int*)GC_malloc(sizeof(int));
    *num1 = 10;
    printf("num1: %d\n", *num1);

    int* num2 = (int*)GC_malloc(sizeof(int));
    *num2 = 20;
    printf("num2: %d\n", *num2);

    // Manual block marking necessary for testing
    ((Block*)num1 - 1)->mark = 1;
    ((Block*)num2 - 1)->mark = 1;

    gc_collect();

    printf("Garbage collector executed.\n");

    return 0;
}
