#include "memory_pool.h"
#include <iostream>
#include <time.h>
#include <vector>
using namespace std;

void test01();//测试分配元素的性能
void test02();//测试分配并释放元素的性能

int main() {
    test01();
    test02();
    return 0;
}

void test01(ull esize, int counts) {
    MemPool* pool = pool_init(esize);
    if (!pool) {
        cerr << "initialize failed" << endl;
        return;
    }

    vector<void*> elements(counts);

    clock_t start = clock();
    for (ull u = 0; u < counts; u++) {
        elements[u] = pool_malloc(pool);
        if (!elements[u]) {
            cerr << "allocate failed" << endl;
            break;
        }
    }

    for (ull w = 0; w < counts; w++) {
        pool_free(pool, elements[w]);
    }

    clock_t end = clock();
    elements.clear();
    pool_destroy(pool);

    clock_t start2 = clock();
    for (ull u = 0; u < counts; u++) elements[u] = (void*)malloc(esize);
    for (ull w = 0; w < counts; w++) elements[u] = free(elements[w]);
    clock_t end2 = clock();

    cout << "Memory Pool costs: " << static_cast<double>(end - start) / CLOCKS_PER_SEC << " seconds" << endl;
    cout << "System Calling costs: " << static_cast<double>(end2 - start2) / CLOCKS_PER_SEC << " seconds" << endl;
    return;
}

void test02(ull esize, int counts) {
    MemPool* pool = pool_init(esize);
    if (!pool) {
        cerr << "initialize failed" << endl;
        return;
    }
    vector<void*> elements(counts);

    clock_t start = clock();
    int turns1 = 0;
    while (turns1 < counts) {
        for (int u = 0; u < 20; u++) {
            elements[u] = pool_malloc(pool);
            if (!elements[u]) {
                cerr << "allocate failed" << endl;
                break;
            }
        }
        for (int w = 0; w < 20; w++) {
            pool_free(pool, elements[w]);
        }
        turns1++;
    }
    clock_t end = clock();
    elements.clear();
    pool_destroy(pool);

    clock_t start2 = clock();

    for (int i = 0; i < counts; i++) {
        elements[i] = (void*)malloc(esize);
        free(elements[i]);
    }
    clock_t end2 = clock();

    cout << "Memory Pool costs: " << static_cast<double>(end - start) / CLOCKS_PER_SEC << " seconds" << endl;
    cout << "System Calling costs: " << static_cast<double>(end2 - start2) / CLOCKS_PER_SEC << " seconds" << endl;
    return;
}
