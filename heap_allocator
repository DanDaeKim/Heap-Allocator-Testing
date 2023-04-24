#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#define MEMORY_POOL_SIZE 4096

typedef struct Block {
    size_t size;
    bool is_free;
    struct Block *next;
} Block;

static char memory_pool[MEMORY_POOL_SIZE];
static Block *free_list = NULL;

void initialize_allocator() {
    free_list = (Block *) memory_pool;
    free_list->size = MEMORY_POOL_SIZE - sizeof(Block);
    free_list->is_free = true;
    free_list->next = NULL;
}

void *my_malloc(size_t size) {
    Block *current = free_list;
    Block *previous = NULL;

    while (current) {
        if (current->is_free && current->size >= size + sizeof(Block)) {
            // Split the current block if there's enough space
            if (current->size >= size + 2 * sizeof(Block)) {
                Block *new_block = (Block *)(((char *) current) + sizeof(Block) + size);
                new_block->size = current->size - size - sizeof(Block);
                new_block->is_free = true;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }
            current->is_free = false;
            return ((char *) current) + sizeof(Block);
        }
        previous = current;
        current = current->next;
    }

    // No suitable block found
    return NULL;
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
