#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern void debug(const char *fmt, ...);
extern void *sbrk(intptr_t increment);

typedef struct block {
    struct block *next;
    int size_used;
    char data[1];
} Block;

Block *head = NULL;

void *myalloc(size_t size) {
    if (size <= 0) {
        return NULL;
    }

    Block *curr = head;
    Block *prev = NULL;

    while (curr != NULL) {
        if (!(curr->size_used & 1) && (curr->size_used >> 1) >= size) {
            curr->size_used = curr->size_used | 1;
            return curr->data;
        }
        prev = curr;
        curr = curr->next;
    }

    curr = sbrk(0);
    void *pData = sbrk(sizeof(Block) + size);

    if (pData == (void*) -1) {
        return NULL;
    }

    curr->size_used = (size << 1) | 1;
    curr->next = NULL;
    if (prev != NULL) {
        prev->next = curr;
    }

    return curr->data;
}

void myfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    Block *curr = (Block*) (ptr - sizeof(Block));
    curr->size_used &= ~1;
}

void *myrealloc(void *ptr, size_t size) {
    if (ptr == NULL) {
        return myalloc(size);
    }

    if (size <= 0) {
        myfree(ptr);
        return NULL;
    }

    Block *curr = (Block*) (ptr - sizeof(Block));
    if ((curr->size_used >> 1) >= size) {
        return ptr;
    }

    void *new_pData = myalloc(size);
    if (new_pData == NULL) {
        return NULL;
    }

    size_t originsize = curr->size_used >> 1;
    if (originsize > size) {
        originsize = size;
    }
    memcpy(new_pData, ptr, originsize);
    myfree(ptr);
    return new_pData;
}