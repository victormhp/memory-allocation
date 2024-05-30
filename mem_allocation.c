#include "mem_allocation.h"

void* base = NULL;

void* dummy_malloc(size_t size)
{
    void* p = sbrk(0);
    if (sbrk(size) == (void*)-1) {
        return NULL;
    }
    return p;
}

t_block find_block(t_block* last, size_t size)
{
    t_block b = base;
    while (b && !(b->free && b->size >= size)) {
        *last = b;
        b = b->next;
    }
    return (b);
}

t_block extend_heap(t_block last, size_t s)
{
    t_block b = sbrk(0);
    if (sbrk(BLOCK_SIZE + s) == (void*)-1) {
        return (NULL);
    }
    b->size = s;
    b->next = NULL;
    if (last) {
        last->next = b;
    }
    b->free = 0;
    return (b);
}

void split_block(t_block b, size_t s)
{
    t_block new = b->data + s;
    new->size = b->size - s - BLOCK_SIZE;
    new->next = b->next;
    new->free = 1;
    b->size = s;
    b->next = new;
}

void* heap_alloc(size_t size)
{
    t_block b, last;
    size_t s = align4(size);
    if (base) {
        last = base;
        b = find_block(&last, s);
        if (b) {
            if ((b->size - s) >= (BLOCK_SIZE + 4)) {
                split_block(b, s);
            }
            b->free = 0;
        } else {
            b = extend_heap(last, s);
            if (!b) {
                return (NULL);
            }
        }
    } else {
        b = extend_heap(NULL, s);
        if (!b) {
            return (NULL);
        }
        base = b;
    }
    return (b->data);
}
