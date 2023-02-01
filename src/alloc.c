#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern void debug(const char *fmt, ...);
extern void *sbrk(intptr_t increment);

unsigned int max_size;

typedef struct block {
    struct block* next;
    size_t size_used;
    char data[1];
} Block;

Block* head = NULL;

void *myalloc(size_t size)
{
    if (size <= 0)
        return NULL;

    Block* curr = head;
    Block* prev = NULL;

    // Look for a block with enough space
    while (curr != NULL)
    {
        if ((curr->size_used & 1) == 0 && (curr->size_used >> 1) >= size)
        {
            // Update the size_used to indicate the block is now in use
            curr->size_used = (size << 1) | 1;
            return curr->data;
        }
        prev = curr;
        curr = curr->next;
    }

    // Allocate a new block if no suitable block was found
    curr = sbrk(0);
    void* pData = sbrk(size + sizeof(Block));

    // Check if allocation failed
    if (pData == (void*) -1)
        return NULL;
    
    curr->size_used = (size << 1) | 1;
    curr->next = NULL;
    if (prev != NULL)
        prev->next = curr;
    
    max_size += size;
    return curr->data;
}

void myfree(void *ptr)
{
    if (ptr == NULL)
        return;
    Block* curr = (Block*) (ptr - sizeof(Block));
    curr->size_used >>= 1;
}

void *myrealloc(void *ptr, size_t size)
{
    // If there is no existing space, allocate new space
    if (ptr == NULL)
        return myalloc(size);
    
    // If size is 0, free the existing space
    if (size <= 0)
    {
        myfree(ptr);
        return NULL;
    }
    
    // Otherwise, allocate new space and copy over the data
    Block *curr = (Block *) (ptr - sizeof(Block));

    // If the existing block has enough space, just return it
    if ((curr->size_used >> 1) >= size)
        return ptr;

    void* new_pData = myalloc(size);
    if (new_pData == NULL)
        return NULL;
    
    size_t old_size = curr->size_used >> 1;
    if (old_size > size)
        old_size = size;
    memcpy(new_pData, ptr, old_size);
    myfree(ptr);
    return new_pData;
}