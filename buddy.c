#include <string.h> //fo memset
#include <unistd.h>
#include <stdio.h>

typedef struct mem_block mem_block;

struct mem_block{
    size_t size; //with offset
    mem_block* parent;
    mem_block* left;
    mem_block* right;
    int avail;
};

#define OFFSET (sizeof(mem_block))

static mem_block* mem = NULL;
static size_t N = 4194304; //65536

int init_mem();
int extend_mem();
mem_block* find_avail_block(size_t size);

void *malloc(size_t size)
{
    //printf("malloc req: %zu\n", size);
    printf("malloc\n");
    if(size <= 0)
        return NULL;

    if(mem == NULL){ //init memory, own method?
        if(init_mem())
            return NULL;
    }

    mem_block *ret = find_avail_block(size+OFFSET);
    //printf("malloc ret: %zu, on addr: %p\n", ret->size, ret);
    return ret+1;
}

void free(void* ptr)
{
    printf("free\n");
    if(!ptr)
        return;

    mem_block* block = (mem_block*)ptr - 1;
    block->avail = 1;
    block = block->parent;
    while(block && block->left->avail && block->right->avail){
        block->avail = 1;
        block = block->parent;
    }
}

void *calloc(size_t n, size_t size)
{
    printf("calloc\n");
    void *ret = malloc(n * size);
    if(!ret)
        return NULL;
    memset(ret, 0, n * size);
    return ret;
}

void *realloc(void *ptr, size_t size)
{
    printf("realloc\n");
    if(!size){
        free(ptr);
        return NULL;
    }
    if(!ptr)
        return malloc(size);

    mem_block* block = (mem_block*)ptr - 1;
    if(block->size >= size)//räcker halva?
        return ptr;

    block->avail = 1;
    block = block->parent;
    while(block && block->left->avail && block->right->avail){
        block->avail = 1;
        if(block->size >= size)
            break;
        block = block->parent;
    }

    if(block && block->size >= size)
        return ptr;

    void *new_ptr = malloc(size);
    if(!new_ptr)
        return NULL;

    memcpy(new_ptr, ptr, ((mem_block*)ptr - 1)->size);
    free(ptr);
    return new_ptr;
}

int init_mem()
{
    mem = sbrk(N);
    if(mem == (void*)(-1))
        return 1;
    //printf("init addr: %p\n", mem);
    mem->size = N;
    mem->parent = NULL; //Head
    mem->left = NULL;
    mem->right = NULL;
    mem->avail = 1;
    return 0;
}

int extend_mem()
{
    printf("extend\n");
    mem_block* mem_parent = sbrk(N+OFFSET);
    if(mem_parent == (void*)(-1))
        return 1;
    //printf("extending: %zu\n", N);
    mem_block* mem_buddy = mem_parent + 1;

    mem_parent->parent = NULL;//new head
    mem_parent->size = 2*N+OFFSET;
    mem_parent->left = mem;
    mem_parent->right = mem_buddy;
    mem_parent->avail = 0;

    mem->parent = mem_parent;

    mem_buddy->size = N;
    mem_buddy->parent = mem_parent;
    mem_buddy->left = NULL;
    mem_buddy->right = NULL;
    mem_buddy->avail = 1;
    N *= 2;
    mem = mem_parent;
    return 0;
}

mem_block* pre_order_lookup(size_t size, mem_block* root)
{
    static int i;
    i++;
    printf("lookup at: %d\n", i);
    mem_block* tmp;
    if(root && root->size >= size){
        if(root->avail){
            return root;
        }
        tmp = pre_order_lookup(size, root->left);
        if(!tmp)
            tmp = pre_order_lookup(size, root->right);
        return tmp;
    }
    return NULL;
}

mem_block* find_avail_block(size_t size) //size = size + OFFSET
{
    mem_block* block = pre_order_lookup(size, mem);
    while(!block){
        if(extend_mem())
            return NULL;
        block = pre_order_lookup(size, mem);
    }

    while((size + OFFSET) <= (block->size/2)){ //split block
        block->left = block + 1;
        block->left->size = block->size/2;
        //printf("new left at: %p\n", block->left);
        block->left->parent = block;
        block->left->left = NULL;
        block->left->right = NULL;
        block->left->avail = 0;

        block->right = (mem_block*)((char*)(block) + block->size/2);
        //printf("new right at: %p\n", block->right);
        block->right->size = block->size/2;
        block->right->parent = block;
        block->right->left = NULL;
        block->right->right = NULL;
        block->right->avail = 1;

        block->avail = 0;
        block = block->left;
        //return block->left;
    }
    block->avail = 0;
    return block;
}
