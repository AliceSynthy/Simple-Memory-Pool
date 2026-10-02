#include "memory_pool.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
using namespace std;

int main(void)
{
    //printf("=== 内存池 Demo ===\n\n");

    /* 1. 初始化 */
    MemPool *pool = pool_init();
    if (!pool) {
        cerr << "内存池初始化失败" << endl;
        return 1;
    }
    cout << "内存池创建成功" << endl;
    cout << "       块大小: " << BLOCK_SIZE << " bytes, 元素大小: " << ELEMENT_SIZE << " bytes" << endl;
    cout << "       每块可容纳元素数: " << pool->blocks[0]->capacity << endl;
    cout << "       预分配块数: " << pool->block_count << endl << endl;

    /* 2. 分配几个元素 */
    cout << "[malloc] 分配 3 个元素..." << endl;
    void *e1 = pool_malloc(pool);
    void *e2 = pool_malloc(pool);
    void *e3 = pool_malloc(pool);

    strcpy((char *)e1, "Hello");
    strcpy((char *)e2, "Memory");
    strcpy((char *)e3, "Pool!");

    cout << "  e1 = " << (char *)e1 << endl;
    cout << "  e2 = " << (char *)e2 << endl;
    cout << "  e3 = " << (char *)e3 << endl;
    cout << "  当前块数: " << pool->block_count << endl << endl;

    /* 3. 释放 e2，验证空闲链表复用 */
    cout << "[free] 释放 e2..." << endl;
    pool_free(pool, e2);
    cout << "  空闲链表头: " << (void *)pool->free_list << endl << endl;

    cout << "[malloc] 再次分配 → 应从空闲链表取..." << endl;
    void *e4 = pool_malloc(pool);
    strcpy((char *)e4, "Reuse!");
    cout << "  e4 = " << (char *)e4 << "  (地址 == e2? " << ((e4 == e2) ? "YES" : "NO") << ")" << endl << endl;

    /* 4. 批量分配，触发新块分配 */
    cout << "[malloc] 批量分配 120000 个元素（触发新块 & 扩容）..." << endl;
    size_t batch = 120000;
    void **ptrs = (void **)malloc(sizeof(void *) * batch);
    for (size_t i = 0; i < batch; i++) {
        ptrs[i] = pool_malloc(pool);
    }
    cout << "  当前块数: " << pool->block_count << " (块数组容量: " << pool->block_cap << ")" << endl;

    /* 5. 释放全部 */
    for (size_t i = 0; i < batch; i++) {
        pool_free(pool, ptrs[i]);
    }
    free(ptrs);

    /* 6. 销毁内存池 */
    cout << "\n[destroy] 销毁内存池..." << endl;
    pool_destroy(pool);
    cout << "  内存池已销毁。" << endl;

    return 0;
}
