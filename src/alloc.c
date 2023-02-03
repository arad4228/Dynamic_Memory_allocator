#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern void debug(const char *fmt, ...);
extern void *sbrk(intptr_t increment);

typedef struct block {
    struct block* next;
    size_t size;
    uint8_t used;
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
        if (!curr->used && curr->size >= size)
        {
            // Update the used field to indicate the block is now in use
            curr->used = 1;
            return curr->data;
        }
        prev = curr;
        curr = curr->next;
    }

    // Allocate a new block if no suitable block was found
    void* pData = sbrk(size + sizeof(Block));

    // Check if allocation failed
    if (pData == (void*) -1)
        return NULL;
    
    curr = (Block*)pData;
    curr->size = size;
    curr->used = 1;
    curr->next = NULL;
    if (prev != NULL)
        prev->next = curr;
    
    return curr->data;
}

void myfree(void *ptr)
{
    if (ptr == NULL)
        return;
    Block* curr = (Block*) (ptr - sizeof(Block));
    curr->used = 0;
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
    Block* curr = (Block*) (ptr - sizeof(Block));
    void* new_ptr = myalloc(size);
    memcpy(new_ptr, ptr, curr->size > size ? size : curr->size);
    myfree(ptr);
    return new_ptr;
}