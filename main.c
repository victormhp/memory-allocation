#include "mem_allocation.h"
#include <stdio.h>

void test_alloc()
{
    size_t size = 1024;
    void* initial_break = sbrk(0);
    printf("Initial break: %p\n", initial_break);

    void* allocated_memory = heap_alloc(size);
    if (allocated_memory == NULL) {
        printf("Memory allocation failed!\n");
    }

    void* new_break = sbrk(0);
    printf("Allocated memory starts at: %p\n", allocated_memory);
    printf("New program break: %p\n", new_break);
    printf("Expected new break: %p\n", (char*)initial_break + size);

    heap_free(allocated_memory);
    printf("Free break: %p\n", initial_break);
}

int main()
{
    test_alloc();
}
