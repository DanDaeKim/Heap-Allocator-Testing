#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>

#define INITIAL_MEMORY_POOL_SIZE 4096

typedef struct Block {
    size_t size;
    bool is_free;
    struct Block *next;
} Block;

static Block *free_list = NULL;

void initialize_allocator() {
    free_list = (Block *) sbrk(INITIAL_MEMORY_POOL_SIZE);
    free_list->size = INITIAL_MEMORY_POOL_SIZE - sizeof(Block);
    free_list->is_free = true;
    free_list->next = NULL;
}

void *my_malloc(size_t size) {
    Block *best_fit = NULL;
    Block *best_fit_prev = NULL;
    Block *current = free_list;
    Block *previous = NULL;

    while (current) {
        if (current->is_free && current->size >= size + sizeof(Block)) {
            if (!best_fit || current->size < best_fit->size) {
                best_fit = current;
                best_fit_prev = previous;
            }
        }
        previous = current;
        current = current->next;
    }

    if (!best_fit) {
        // Increase the size of the memory pool
        Block *new_block = (Block *) sbrk(size + sizeof(Block));
        new_block->size = size;
        new_block->is_free = false;
        new_block->next = NULL;
        if (previous) {
            previous->next = new_block;
        }
        return ((char *) new_block) + sizeof(Block);
    }

    // Split the best-fit block if there's enough space
    if (best_fit->size >= size + 2 * sizeof(Block)) {
        Block *new_block = (Block *)(((char *) best_fit) + sizeof(Block) + size);
        new_block->size = best_fit->size - size - sizeof(Block);
        new_block->is_free = true;
        new_block->next = best_fit->next;

        best_fit->size = size;
        best_fit->next = new_block;
    }
    best_fit->is_free = false;
    return ((char *) best_fit) + sizeof(Block);
}

void my_free(void *ptr) {
    if (!ptr) {
        return;
    }

    Block *block_to_free = (Block *)(((char *) ptr) - sizeof(Block));
    block_to_free->is_free = true;

    // Coalesce adjacent free blocks
    Block *current = free_list;
    while (current->next) {
        if (current->is_free && current->next->is_free) {
            current->size += current->next->size + sizeof(Block);
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

int main() {
    initialize_allocator();

    void *ptr1 = my_malloc(100);
    void *ptr2 = my_malloc(200);

    printf("Allocated blocks: %p, %p\n", ptr1, ptr2);

    my_free(ptr1);
    my_free(ptr2);

    return 0;
}
