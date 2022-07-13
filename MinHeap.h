#pragma  once

/**
 * 二叉堆(最小堆)
 *
 */
#include <iomanip>
#pragma once
#include <iostream>
using namespace std;

template <class T>
class MinHeap{

public:
	
    private:
        T *mHeap[1024000];        // 数据
		int mSize;        // 实际容量
        int mCapacity;    // 总的容量
       

    private:
        // 最小堆的向下调整算法
        void filterdown(int start, int end);
        // 最小堆的向上调整算法(从start开始向上直到0，调整堆)
        void filterup(int start);
    public:
        int length();
        MinHeap();
        MinHeap(int capacity);
        ~MinHeap();

        // 返回data在二叉堆中的索引
        int getIndex(T * data);
        // 删除最小堆中的data
        int remove(T * data);
		// 弹出最小堆中堆顶的元素
		T* pop();
        // 将data插入到二叉堆中
        int insert(T * data);
        // 打印二叉堆
        void print();
};
