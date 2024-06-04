#ifndef MEM_ALLOCATION
#define MEM_ALLOCATION

#include <sys/types.h>
#include <unistd.h>

#define align4(x) ((((x - 1) << 2) >> 2) - 4)
#define BLOCK_SIZE 12

typedef struct s_block* t_block;

struct s_block {
    size_t size;
    t_block next;
    t_block prev;
    int free;
    void* ptr;
    char data[1];
};

void* dummy_malloc(size_t size);

t_block get_block(void* p);

int valid_addr(void* p);

t_block find_block(t_block* last, size_t size);

t_block extend_heap(t_block last, size_t size);

void split_block(t_block b, size_t s);

void copy_block(t_block src, t_block dst);

void* heap_alloc(size_t size);

void* heap_calloc(size_t number, size_t size);

void heap_free(void* p);

void* heap_realloc(void* p, size_t size);

#endif
