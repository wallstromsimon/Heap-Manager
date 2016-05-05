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

mem_block *new_block(mem_block* end, size_t size)
{
    mem_block *block;
    block = sbrk(size + OFFSET);
    if(block == (void*) - 1)
        return NULL;

    if(end)
        end->next = block;

    block->size = size;
    block->next = NULL;
    block->avail = 0;
    return block;
}

mem_block *try_reuse_block(mem_block **end, size_t size)
{
    mem_block *current = mem;
    while(current && !(current->avail && current->size >= size)){
        *end = current;
        current = current->next;
    }
    if(!current){
        current = new_block(*end, size);
    }else{
        current->avail = 0;
    }
    return current;
}

void *malloc(size_t size)
{
    if(size <= 0)
        return NULL;
    mem_block *block;
    if(!mem){
        block = new_block(NULL, size);
        mem = block;
    }else{
        mem_block *end = mem;
        block = try_reuse_block(&end, size);
    }
//    if(!block)
//        return NULL;
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

    //Test if ((mem_bloc*)ptr+((mem_block*)ptr - 1)->size)->size räcker & avail
    /*mem_block *next = ((mem_block*)ptr-1)->next;
    if(next && (char*)(ptr + size) == (char*)next && next->avail == 1 && ((((mem_block*)ptr - 1)->size + next->size + 1) >= size)){
        printf("merg\n");
        next->avail = 0;
        ((mem_block*)ptr-1)->size += next->size + 1;
        ((mem_block*)ptr-1)->next = next->next;
        return ptr;
    }*/

    void *new_ptr = malloc(size);
    if(!new_ptr){
        return NULL;
    }
    memcpy(new_ptr, ptr, ((mem_block*)ptr - 1)->size);
    free(ptr);
    return new_ptr;
}

void *calloc(size_t n, size_t size)
{
    void *ret = malloc(n * size);
    if(!ret){
        return NULL;
    }
    memset(ret, 0, n * size);
    return ret;
}
