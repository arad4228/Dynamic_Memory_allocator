#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define POOL_SIZE 1024*1024*1024

extern void debug(const char *fmt, ...);
extern void *sbrk(intptr_t increment);

unsigned int max_size;
static char memory_pool[POOL_SIZE];
static int pool_index = 0;

typedef struct Block Block;
struct Block {
    size_t size;
    int is_used;
    Block *next;
};

static Block *allocated_list = NULL;

void add_block_to_allocated_list(Block *block) {
    block->next = allocated_list;
    allocated_list = block;
}

void *myalloc(size_t size)
{
    if (pool_index + size > POOL_SIZE) {
        // handle error, out of memory
        return NULL;
    }
    void *result = &memory_pool[pool_index];
    pool_index += size;
    return result;
}

void myfree(void *ptr)
{
    if(ptr == NULL) return;
    Block *current = allocated_list;
    Block *prev = NULL;
    while (current != NULL) {
        if((void *)((uint8_t*)current + sizeof(Block)) == ptr){
            current->is_used = 0;
            if(prev == NULL)
                allocated_list = current->next;
            else
                prev->next = current->next;
            break;
        }
        prev = current;
        current = current->next;
    }
}

void *myrealloc(void *ptr, size_t size)
{
    if (ptr == NULL) {
        return myalloc(size);
    }
    if (size == 0) {
        myfree(ptr);
        return NULL;
    }
    void *new_ptr = myalloc(size);
    if (new_ptr == NULL) {
        // handle error
        return NULL;
    }
    memcpy(new_ptr, ptr, size);
    myfree(ptr);
    return new_ptr;
}