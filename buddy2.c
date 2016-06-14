#include <string.h> //fo memset
#include <unistd.h>
#include <stdio.h>

typedef struct mem_block mem_block;

struct mem_block{
    unsigned avail:1;
    char kval;
    mem_block* next;
    mem_block* prev;
};

#define OFFSET (sizeof(mem_block)) //24 on my comp
#define N (30) //2^22 = 4194304

static mem_block* mem = NULL;
static mem_block* freelist[(N+1)];

int init_mem()
{
    mem = sbrk(1<<N);
    if(mem == (void*)(-1))
        return 1;
    mem->avail = 1;
    mem->kval = N;
    mem->next = NULL;
    mem->prev = NULL;
    freelist[N] = mem;
    return 0;
}

size_t nextpow2(size_t size)
{
    //only works on x86, gcc may fix it otherwise
    //clz: count leading zeros
    return (32 - __builtin_clz(size-1));
}

/*adjust the size to be a multiple by 2. The number of shifts are returned in index*/
void adjust_size(size_t size, size_t* index)
{
    size_t new_size = 1;
    *index = 0;
    while(new_size < size + OFFSET){
        ++(*index);
        new_size  = 1<<*index;
    }
}

mem_block* take_free_block(size_t index)
{
    mem_block* block = freelist[index];
    if(block){
        freelist[index] = block->next;
        if(freelist[index])
            freelist[index]->prev = NULL;
        block->next = NULL;
        block->prev = NULL; //borde redan vara null
    }
    return block;
}

void add_to_freelist(mem_block* block)
{
    mem_block* head = freelist[block->kval];
    block->avail = 1;
    if(head){
        block->next = head;
        head->prev = block;
    }else{ //just add
        block->next = NULL;
    }
    block->prev = NULL;
    freelist[block->kval] = block;
}

void* malloc(size_t size)
{
    //printf("malloc\n");
    if(size <= 0)
        return NULL;
    if(!mem){
        if(init_mem())
            return NULL;
    }
    size_t k = nextpow2(size+OFFSET);
    //Sanity check
    size_t k2;
    adjust_size(size, &k2);
    if(k != k2){
        printf("k = %zu, k2 = %zu\n",k,k2 );
    }

    if(k > N)
        return NULL; //Not enough mem
    size = 1<<k; //Increase size to be the smallest power of two: size = 2^K
    //printf("malloc lvl %zu mapped to %zu\n", k, size);
    mem_block* block = take_free_block(k);

    if(block){
        block->avail = 0;
        //printf("take block at: %p\n", block);
        return block+1;
    }
    //Find the first list J with an available block; J >= K
    size_t i = k;
    while(!freelist[i]){
        i++;
        if(i>N)
            return NULL; // If no such J exists, then return NULL.
    }
    //printf("found free at lvl: %zu\n", i);
    //For each I : K < I < J split the block in two pieces, set their kval,
    //reserved, succ and pred attributes, and put them into freelist[I − 1]
    while(i > k){ // > or >=
        block = take_free_block(i); //should never be null
        i--;
        block->kval = i;
        block->avail = 1;
        block->prev = NULL;

        mem_block* buddy = (mem_block*)((char*)(block) + (1<<i));
        buddy->kval = i;
        buddy->avail = 1;
        buddy->prev = block;
        buddy->next = NULL;

        block->next = buddy;

        freelist[i] = block;
    }

    block = take_free_block(k);
    block->avail = 0;
    //printf("take block at: %p, avail: %d\n", block, block->avail);
    return block+1;

}

mem_block* merge_blocks(mem_block* block)//find and return buddy
{
    mem_block* buddy = (mem_block*)((char*)mem + (((char*)block - (char*)mem) ^ (1 << block->kval)));
    if(block->kval != N && buddy->avail && (block->kval == buddy->kval)){ //if buddy, try to free the new block
        //printf("avail buddy at lvl: %d\n", block->kval);
        if(freelist[buddy->kval] == buddy){
        	if(buddy->next){
                buddy->next->prev = NULL;
                freelist[buddy->kval] = buddy->next;
        	}else{
                freelist[buddy->kval] = NULL;
        	}
        }else{
    		if(buddy->next)
                buddy->next->prev = buddy->prev;
    		if(buddy->prev)
                buddy->prev->next = buddy->next;
        }
        buddy->prev = NULL;
        buddy->next = NULL;
        block = block < buddy ? block : buddy;
        block->kval += 1;
        merge_blocks(block);
    }
    return block;
}

void free(void *ptr)
{
    //printf("free\n");
    if(!ptr)
        return;
    mem_block* block = (mem_block*)ptr - 1;
    block->avail = 1;
    if(block->kval <= N){
        block = merge_blocks(block);
    }
    block->avail = 1;
    add_to_freelist(block);
    return;
}

void *realloc(void *ptr, size_t size)
{
    //printf("realloc\n");
    if(!size){
        free(ptr);
        return NULL;
    }
    if(!ptr)
        return malloc(size);

    size_t k = nextpow2(size+OFFSET);

    if(((mem_block*)ptr - 1)->kval >= k);
        return ptr;

    //Check if buddy is free

    void *new_ptr = malloc(size);
    if(!new_ptr){
        return NULL;
    }

    memcpy(new_ptr, ptr, (1<<(((mem_block*)ptr - 1)->kval)) - OFFSET);
    free(ptr);
    return new_ptr;
}

void *calloc(size_t n, size_t size)
{
    //printf("calloc\n");
    void *ret = malloc(n * size);
    if(!ret)
        return NULL;

    memset(ret, 0, n * size);
    return ret;
}
