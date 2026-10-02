#include "memory_pool.h"
#include <iostream>
#include <time.h>
using namespace std;

void test_memory_pool01();//测试分配元素的性能
void test_memory_pool02();//测试分配并释放元素的性能

int main() {
    test_memory_pool01();
    test_memory_pool02();
    return 0;
}

void test_memory_pool01() {
    MemPool *pool = pool_init();
    if (!pool) {
        cerr << "内存池初始化失败" << endl;
        return;
    }

    const size_t num_elements = 1000000; // 分配一百万个元素
    void **elements = new void*[num_elements];

    clock_t start = clock();
    for (size_t i = 0; i < num_elements; ++i) {
        elements[i] = pool_malloc(pool);
        if (!elements[i]) {
            cerr << "内存分配失败" << endl;
            break;
        }
    }
    clock_t end = clock();
     cout << "内存池分配 " << num_elements << " 个元素耗时: " << static_cast<double>(end - start) / CLOCKS_PER_SEC << "秒" << endl;
    
    // 释放所有元素
    for (size_t i = 0; i < num_elements; ++i) {
        pool_free(pool, elements[i]);
    }

    delete[] elements;
    pool_destroy(pool);

    clock_t start2 = clock();
    for (size_t i = 0; i < num_elements; ++i) {
        elements[i] = (void*)malloc(ELEMENT_SIZE);
        if (!elements[i]) {
            cerr << "内存分配失败" << endl;
            break;
        }
    }
    clock_t end2 = clock();
    cout << "标准库分配 " << num_elements << " 个元素耗时: ";
    cout << static_cast<double>(end2 - start2) / CLOCKS_PER_SEC;
    cout << " 秒" << endl;
}

void test_memory_pool02() {
    MemPool *pool = pool_init();
    if (!pool) {
        cerr << "内存池初始化失败" << endl;
        return;
    }

    const size_t num_elements = 1000000; // 分配一百万个元素
    void **elements = new void*[num_elements];

    clock_t start = clock();
    for (size_t i = 0; i < num_elements; ++i) {
        elements[i] = pool_malloc(pool);
        if (!elements[i]) {
            cerr << "内存分配失败" << endl;
            break;
        }
    }
    for (size_t i = 0; i < num_elements; ++i) {
        pool_free(pool, elements[i]);
    }
    clock_t end = clock();
    cout << "内存池分配并释放 " << num_elements << " 个元素耗时: "<< static_cast<double>(end - start) / CLOCKS_PER_SEC<< " 秒" << endl;
              

    delete[] elements;
    pool_destroy(pool);

    clock_t start2 = clock();
    for (size_t i = 0; i < num_elements; ++i) {
        elements[i] = (void*)malloc(ELEMENT_SIZE);
        if (!elements[i]) {
            cerr << "内存分配失败" << endl;
            break;
        }
    }
    for (size_t i = 0; i < num_elements; ++i) {
        free(elements[i]);
    }
    clock_t end2 = clock();
    cout << "标准库分配并释放 " << num_elements << " 个元素耗时: " << static_cast<double>(end2 - start2) / CLOCKS_PER_SEC << " 秒" << endl;
}

