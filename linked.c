#include <string.h>
#include <unistd.h>

typedef struct mem_block mem_block;
struct mem_block {
    size_t size; //Without offset
    mem_block *next;
    int avail;
};
#define OFFSET sizeof(mem_block)

static mem_block *mem = NULL;

void *malloc(size_t size)
{
    if(size <= 0)
        return NULL;

    mem_block *block = mem;
    while(block && !(block->avail && block->size >= size)){
        block = block->next;
    }
    if(!block){ //first or no avail block
        block = sbrk(size + OFFSET);
        if(block == (void*) - 1)
            return NULL;

        if(mem)
            mem->next = block;

        block->size = size;
        block->next = NULL;
    }

    block->avail = 0;
    mem = block; //update mem pointer
    return block + 1;
}

void free(void *ptr)
{
    if(!ptr)
        return;
    ((mem_block*)ptr - 1)->avail = 1;
}

void *realloc(void *ptr, size_t size)
{
    if(!size){
        free(ptr);
        return NULL;
    }
    if(!ptr)
        return malloc(size);

    if(((mem_block*)ptr - 1)->size >= size)
        return ptr;

    void *new_ptr = malloc(size);
    if(!new_ptr)
        return NULL;

    memcpy(new_ptr, ptr, ((mem_block*)ptr - 1)->size);
    free(ptr);
    return new_ptr;
}

void *calloc(size_t n, size_t size)
{
    void *ret = malloc(n * size);
    if(!ret)
        return NULL;

    memset(ret, 0, n * size);
    return ret;
}
