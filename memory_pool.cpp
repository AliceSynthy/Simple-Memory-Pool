#include "data structure.h"
#include <cstdlib>

static Block* BlockCreate(ull esize, ull bsize) {
    Block* blk = (Block*)malloc(sizeof(Block));
    if (!blk) return nullptr;

    blk->data = (char*)malloc(bsize);
    if (!blk->data) {
        free(blk);
        return nullptr;
    }

    blk->capacity = bsize / esize;
    blk->used = 0;
    return blk;
}

MemPool* pool_init(ull esize, ull bsize) {
    MemPool* pool = (MemPool*)malloc(sizeof(MemPool));
    if (!pool) return nullptr;

    pool->elemsize = esize;
    pool->blocksize = bsize;
    pool->freelist = nullptr;
    pool->usedblock = 0;
    pool->blockcap = INITBLOCKS;

    pool->blocks = (Block**)malloc(sizeof(Block*) * pool->blockcap);
    if (!pool->blocks) {
        free(pool);
        return nullptr;
    }

    Block* first = BlockCreate(esize, bsize);
    if (!first) {
        free(pool->blocks);
        free(pool);
        return nullptr;
    }

    pool->blocks[pool->usedblock++] = first;
    return pool;
}

void* pool_malloc(MemPool* pool) {
    if (!pool) return nullptr;

    pool->mtx.lock();

    if (pool->freelist) {
        FreeNode* node = pool->freelist;
        pool->freelist = node->next;
        return (void*) node;
    }

    Block* cur = pool->blocks[pool->usedblock - 1];
    if (cur->used < cur->capacity) {
        void* elem = cur->data + cur->used * pool->elemsize;
        cur->used++;
        return elem;
    }

    if (pool->usedblock == pool->blockcap) {
        ull newcap = pool->blockcap * 2;
        Block** temp = (Block**)realloc(pool->blocks, sizeof(Block*) * newcap);
        if (!temp) return nullptr;
        pool->blocks = temp;
        pool->blockcap = newcap;
    }

    Block* newblk = BlockCreate(pool->elemsize, pool->blocksize);
    if (!newblk) return nullptr;
    pool->blocks[pool->usedblock++] = newblk;

    void* elem = newblk->data + newblk->used * pool->elemsize;
    newblk->used++;

    pool->mtx.unlock();
    return elem;
}

void pool_free(MemPool* pool, void* elem) {
    if (!pool || !elem) return;

    pool->mtx.lock();

    FreeNode* node = (FreeNode*)elem;
    node->next = pool->freelist;
    pool->freelist = node;

    pool->mtx.unlock();

    return;
}

void pool_destroy(MemPool* pool) {
    if (!pool) return;

    pool->mtx.lock();

    for (ull i = 0; i < pool->usedblock; i++) {
        free(pool->blocks[i]->data);
        free(pool->blocks[i]);
    }
    free(pool->blocks);

    pool->mtx.unlock();
    free(pool);
    return;
}
