#include "memory_pool.h"
#include <stdlib.h>
#include <string.h>

/* ---------- 内部：分配一个新块 ---------- */
static Block *block_create(size_t block_size, size_t elem_size)
{
    Block *blk = (Block *)malloc(sizeof(Block));
    if (!blk) return NULL;

    blk->data = (char *)malloc(block_size);
    if (!blk->data) { free(blk); return NULL; }

    blk->capacity = block_size / elem_size;
    blk->used = 0;
    return blk;
}

/* ---------- 模块一：初始化 ---------- */
MemPool *pool_init(size_t elem_size, size_t block_size)
{
    MemPool *pool = (MemPool *)malloc(sizeof(MemPool));
    if (!pool) return NULL;

    pool->elem_size   = elem_size;
    pool->block_size  = block_size;
    pool->free_list   = NULL;
    pool->block_count = 0;
    pool->block_cap   = INITIAL_BLOCKS;

    pool->blocks = (Block **)malloc(sizeof(Block *) * pool->block_cap);
    if (!pool->blocks) { free(pool); return NULL; }

    /* 预分配首个内存块 */
    Block *first = block_create(block_size, elem_size);
    if (!first) { free(pool->blocks); free(pool); return NULL; }

    pool->blocks[pool->block_count++] = first;
    return pool;
}

/* ---------- 模块二：内存分配（三级判断分支） ---------- */
void *pool_malloc(MemPool *pool)
{
    if (!pool) return NULL;

    /*
     * Step1: 空闲链表非空？
     *   是 → 取链表头元素，O(1) 返回
     */

    pool->mtx.lock();  /* 加锁保护共享资源 */

    if (pool->free_list) {
        FreeNode *node = pool->free_list;
        pool->free_list = node->next;
        return (void *)node;
    }

    /*
     * Step2: 当前块有空闲元素？
     *   是 → 返回当前块中下一个未用元素
     */
    Block *cur = pool->blocks[pool->block_count - 1];
    if (cur->used < cur->capacity) {
        void *elem = cur->data + cur->used * pool->elem_size;
        cur->used++;
        return elem;
    }

    /*
     * Step3: 需要新块
     *   已使用块数 == 容量上限 → 扩容 2 倍
     */
    if (pool->block_count == pool->block_cap) {
        size_t new_cap = pool->block_cap * 2;
        Block **tmp = (Block **)realloc(pool->blocks, sizeof(Block *) * new_cap);
        if (!tmp) return NULL;
        pool->blocks   = tmp;
        pool->block_cap = new_cap;
    }

    Block *new_blk = block_create(pool->block_size, pool->elem_size);
    if (!new_blk) return NULL;

    pool->blocks[pool->block_count++] = new_blk;

    void *elem = new_blk->data + new_blk->used * pool->elem_size;
    new_blk->used++;

    pool->mtx.unlock();  /* 解锁 */
    return elem;
}

/* ---------- 模块三：释放元素（头插法归还空闲链表） ---------- */
void pool_free(MemPool *pool, void *elem)
{
    if (!pool || !elem) return;

    pool->mtx.lock();  

    FreeNode *node = (FreeNode *)elem;
    node->next     = pool->free_list;   /* 头插法 */
    pool->free_list = node;
    pool->mtx.unlock();  
}

/* ---------- 模块四：释放整个内存池 ---------- */
void pool_destroy(MemPool *pool)
{
    if (!pool) return;

    pool->mtx.lock();  

    for (size_t i = 0; i < pool->block_count; i++) {
        free(pool->blocks[i]->data);
        free(pool->blocks[i]);
    }
    free(pool->blocks);
    pool->mtx.unlock();  
    free(pool);
}
