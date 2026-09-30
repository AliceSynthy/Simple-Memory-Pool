#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#if defined(__has_include)
#  if __has_include(<stddef.h>)
#    include <stddef.h>
#  else
typedef __SIZE_TYPE__ size_t;
#  endif
#else
#  include <stddef.h>
#endif

#define BLOCK_SIZE      (4 * 1024 * 1024)  /* 每个内存块大小4MB */
#define ELEMENT_SIZE    32                  /* 每个元素大小32字节 */
#define INITIAL_BLOCKS  4                   /* 初始块上限为4块 */

/* 空闲链表节点：嵌入在空闲元素内部 */
typedef struct FreeNode {
    struct FreeNode *next;
} FreeNode;

/* 内存块 */
typedef struct {
    char *data;          /* 块起始地址 */
    size_t used;         /* 已分配元素数 */
    size_t capacity;     /* 块可容纳元素总数 */
} Block;

/* 内存池 */
typedef struct {
    Block **blocks;      /* 块数组（动态扩容） */
    size_t block_count;  /* 已使用块数 */
    size_t block_cap;    /* 块数组容量上限 */
    FreeNode *free_list; /* 空闲链表头 */
    size_t elem_size;    /* 元素大小 */
    size_t block_size;   /* 块大小 */
} MemPool;

/* 四大核心模块 */
MemPool *pool_init(void);
void    *pool_malloc(MemPool *pool);
void     pool_free(MemPool *pool, void *elem);
void     pool_destroy(MemPool *pool);

#endif
