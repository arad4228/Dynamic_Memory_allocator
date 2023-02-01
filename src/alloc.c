#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern void debug(const char *fmt, ...);
extern void *sbrk(intptr_t increment);

unsigned int max_size;

unsigned int max_size;

typedef struct block {
    struct block *next;
    size_t size_used;
    char data[1];
} Block;

Block *head = NULL;

void *myalloc(size_t size)
{
    if (size <= 0)
        return NULL;

    Block *curr = head;
    Block *prev = NULL;

    while (curr != NULL) {
        if ((curr->size_used & 0x01) == 0 && (curr->size_used >> 1) >= size) {
            curr->size_used |= 0x01;
            return curr->data;
        }
        prev = curr;
        curr = curr->next;
    }

    curr = sbrk(0);
    void *pData = sbrk(size + sizeof(Block));
    if (pData == (void *) -1)
        return NULL;

    curr->size_used = ((size << 1) | 0x01);
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

    Block *curr = (Block *) (ptr - sizeof(Block));
    curr->size_used &= ~0x01;
}

void *myrealloc(void *ptr, size_t size)
{
    if (ptr == NULL)
        return myalloc(size);

    if (size <= 0) {
        myfree(ptr);
        return NULL;
    }

    Block *curr = (Block *) (ptr - sizeof(Block));
    if ((curr->size_used >> 1) >= size)
        return ptr;

    void *new_pData = myalloc(size);
    if (new_pData == NULL)
        return NULL;

    size_t origin_size = curr->size_used >> 1;
    origin_size = (origin_size < size) ? origin_size : size;
    memcpy(new_pData, ptr, origin_size);
    myfree(ptr);
    return new_pData;
}