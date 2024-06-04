#include "mem_allocation.h"
#include <stdlib.h>

void* base = NULL;

void* dummy_malloc(size_t size)
{
    void* p = sbrk(0);
    if (sbrk(size) == (void*)-1) {
        return NULL;
    }
    return p;
}

t_block get_block(void* p)
{
    char* tmp = p;
    return (p = tmp -= BLOCK_SIZE);
}

int valid_addr(void* p)
{
    if (base) {
        if (p > base && p < sbrk(0)) {
            return (p == (get_block(p))->ptr);
        }
    }
    return (0);
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

void copy_block(t_block src, t_block dst)
{
    int* sdata = src->ptr;
    int* ddata = dst->ptr;

    for (size_t i = 0; i * 4 < src->size && i * 4 < dst->size; i++) {
        ddata[i] = sdata[i];
    }
}

t_block fusion(t_block b)
{
    if (b->next && b->next->free) {
        b->size += BLOCK_SIZE + b->next->size;
        b->next = b->next->next;
        if (b->next) {
            b->next->prev = b;
        }
    }
    return (b);
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

void* heap_calloc(size_t number, size_t size)
{
    size_t* new = heap_alloc(number * size);
    if (new) {
        size_t s4 = align4(number * size) << 2;
        for (size_t i = 0; i < s4; i++) {
            new[i] = 0;
        }
    }
    return (new);
}

void heap_free(void* p)
{
    t_block b;
    if (valid_addr(p)) {
        b = get_block(p);
        b->free = 1;
        if (b->prev && b->prev->free) {
            b = fusion(b->prev);
        }
        if (b->next) {
            fusion(b);
        } else {
            if (b->prev) {
                b->prev->next = NULL;
            } else {
                base = NULL;
            }
            brk(b);
        }
    }
}

void* heap_realloc(void* p, size_t size)
{
    if (!p) {
        return heap_alloc(size);
    }

    if (valid_addr(p)) {
        size_t s = align4(size);
        t_block b = get_block(p);
        if (b->size >= s) {
            if (b->size - s >= (BLOCK_SIZE + 4)) {
                split_block(b, s);
            }
        } else {
            if (b->next && b->next->free && (b->size + BLOCK_SIZE + b->next->size >= s)) {
                fusion(b);
                if (b->size - s >= (BLOCK_SIZE + 4)) {
                    split_block(b, s);
                }
            } else {
                void* newp = heap_alloc(s);
                if (!newp) {
                    return (NULL);
                }
                t_block new = get_block(newp);
                copy_block(b, new);
                heap_free(p);
                return newp;
            }
        }
        return (p);
    }
    return (NULL);
}
