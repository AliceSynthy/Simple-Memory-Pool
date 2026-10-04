#include <mutex>

const int BLOCKSIZE = 4 * 1024 * 1024;
const int ELEMSIZE = 32;
const int INITBLOCKS = 4;

typedef unsigned long long ull;

typedef struct FreeNode{
    struct FreeNode* next;
} FreeNode;

typedef struct Block{
    char* data;
    ull used;
    ull capacity;
} Block;

typedef struct MemPool{
    Block** blocks;
    ull usedblock;
    ull blockcap;
    FreeNode* freelist;
    ull elemsize;
    ull blocksize;
    std::mutex mtx;
} MemPool;

MemPool* pool_init(ull esize = ELEMSIZE, ull bsize = BLOCKSIZE);
void* pool_malloc(MemPool* pool);
void pool_free(MemPool* pool, void* elem);
void pool_destroy(MemPool* pool);
