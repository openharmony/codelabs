/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "sort_class.h"
#include "bank_queue_system.h"
#include "native_log_wrapper.h"
#include <string>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <functional>
#include <algorithm>
#include <random>
#include <thread>
#define ZERO 0
#define TWO 2
#define THREE 3
#define TEN 10
#define FIFTY 50
#define ONE_HUNDRED 100
#define FIVE_THOUSAND 5000
#define MAX_MATRIX_SIZE        1000
#define RET_SUCCESS_6         6
using namespace std;

// 常量定义
namespace {
    constexpr int SLEEP_TIME_US = 100 * 1000; // 100ms
    constexpr int DEFAULT_QUEUE_PRIORITY = 0;
    constexpr int SMALL_ARRAY_THRESHOLD = 16;
    constexpr int MEDIUM_ARRAY_THRESHOLD = 100;
    constexpr int MAX_BUCKET_COUNT = 1000;
    constexpr int MIN_BUCKET_COUNT = 10;
    constexpr int DEFAULT_RADIX = 256;
    constexpr int ASCII_CHAR_COUNT = 256;
    constexpr int ALPHABET_SIZE = 26;
    constexpr int TEST_SIZE_SMALL = 1000;
    constexpr int TEST_SIZE_MEDIUM = 10000;
    constexpr int TEST_SIZE_LARGE = 50000;
    constexpr int TEST_SIZE_XLARGE = 100000;
    constexpr int TEST_MAX_VALUE = 1000000;
    constexpr int DECIMAL_BASE = 10;
    constexpr int DEFAULT_SORT_VERSION = 25;
}

SortClass::SortClass(uint32_t version)
{
    LOGI("SortClass::SortClass %d", version);
}

SortClass::~SortClass()
{
    LOGI("FunctionClass::~FunctionClass");
}

// Lomuto分区法 - 简单但效率较低
int LomutoPartition(vector<int>& arr, int low, int high)
{
    int pivot = arr[high];  // 选择最后一个元素作为基准
    int i = low - 1;  // 较小元素的索引
    for (int j = low; j < high; j++) {
        // 如果当前元素小于或等于基准
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;  // 返回基准的最终位置
}

// 递归快速排序（Lomuto版本）
void QuickSortLomuto(vector<int>& arr, int low, int high)
{
    if (low < high) {
        int pi = LomutoPartition(arr, low, high);
        QuickSortLomuto(arr, low, pi - 1);
        QuickSortLomuto(arr, pi + 1, high);
    }
}

void BankBusiness()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw ordinary customer");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试基础Lomuto版本
    auto start = chrono::high_resolution_clock::now();
    QuickSortLomuto(arr, 0, arr.size() - 1);
    auto end = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  Lomuto版本: " << duration.count() << " 微秒" << endl;
}

// Hoare分区法 - 效率更高，交换次数更少
int HoarePartition(vector<int>& arr, int low, int high)
{
    int pivot = arr[low + (high - low) / 2];  // 选择中间元素作为基准
    int i = low - 1;
    int j = high + 1;

    while (i < j) {
        // 从左向右找到第一个大于等于基准的元素
        do {
            i++;
        } while (arr[i] < pivot);

        // 从右向左找到第一个小于等于基准的元素
        do {
            j--;
        } while (arr[j] > pivot);

        // 如果指针相遇或交叉，返回分区点
        if (i >= j) {
            return j;
        }

        // 交换这两个元素
        swap(arr[i], arr[j]);
    }
}

// 递归快速排序（Hoare版本）
void QuickSortHoare(vector<int>& arr, int low, int high)
{
    if (low < high) {
        int pi = HoarePartition(arr, low, high);
        QuickSortHoare(arr, low, pi);      // 注意：这里包含pi
        QuickSortHoare(arr, pi + 1, high);
    }
}

void BankBusinessVIP()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试Hoare版本
    auto start = chrono::high_resolution_clock::now();
    QuickSortHoare(arr, 0, arr.size() - 1);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  Hoare版本:  " << duration.count() << " 微秒" << endl;
}

// 插入排序 - 用于小数组优化
void InsertionSort(vector<int>& arr, int low, int high)
{
    for (int i = low + 1; i <= high; i++) {
        int key = arr[i];
        int j = i - 1;

        while (j >= low && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// 随机选择基准元素，避免最坏情况
int MedianOfThree(vector<int>& arr, int low, int high)
{
    int mid = low + (high - low) / 2;

    // 对三个元素进行排序
    if (arr[low] > arr[mid]) {
        swap(arr[low], arr[mid]);
    }
    if (arr[low] > arr[high]) {
        swap(arr[low], arr[high]);
    }
    if (arr[mid] > arr[high]) {
        swap(arr[mid], arr[high]);
    }

    // 将中位数放在high-1位置
    swap(arr[mid], arr[high - 1]);
    return arr[high - 1];
}

// 三向切分快速排序（应对大量重复元素）
void ThreeWayQuickSort(vector<int>& arr, int low, int high)
{
    // 小数组优化：当数组大小小于等于16时使用插入排序
    if (high - low <= SMALL_ARRAY_THRESHOLD) {
        InsertionSort(arr, low, high);
        return;
    }

    // 使用三数取中法选择基准
    int pivot = MedianOfThree(arr, low, high);

    // 三向切分
    int lt = low;      // arr[low..lt-1] < pivot
    int gt = high - 1; // arr[gt+1..high] > pivot
    int i = low + 1;   // arr[lt..i-1] == pivot

    while (i <= gt) {
        if (arr[i] < pivot) {
            swap(arr[lt], arr[i]);
            lt++;
            i++;
        } else if (arr[i] > pivot) {
            swap(arr[i], arr[gt]);
            gt--;
        } else {
            i++;
        }
    }

    // 递归排序小于和大于基准的部分
    ThreeWayQuickSort(arr, low, lt - 1);
    ThreeWayQuickSort(arr, gt + 1, high);
}

// 快速排序包装函数（推荐使用）
void QuickSort(vector<int>& arr)
{
    if (arr.size() <= 1) {
        return;
    }

    // 如果数组已基本有序，先打乱顺序
    ThreeWayQuickSort(arr, 0, arr.size() - 1);
}

void BankBusinessNew11()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试优化版本
    auto start = chrono::high_resolution_clock::now();
    QuickSort(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  优化版本:  " << duration.count() << " 微秒" << endl;
}

// 基础冒泡排序
void BubbleSortBasic(vector<int>& arr)
{
    int n = arr.size();

    for (int i = 0; i < n - 1; i++) {
        // 每轮将最大的元素"冒泡"到末尾
        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

void BankBusinessBaseBubble()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试基础冒泡版本
    auto start = chrono::high_resolution_clock::now();
    BubbleSortBasic(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  冒泡基础版本:  " << duration.count() << " 微秒" << endl;
}

// 优化版本1：增加提前结束判断
void BubbleSortOptimized(vector<int>& arr)
{
    int n = arr.size();

    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;  // 标记本轮是否发生交换

        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }

        // 如果本轮没有发生交换，说明数组已经有序
        if (!swapped) {
            break;  // 提前结束排序
        }
    }
}

void BankBusinessOptimizeBubble1()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试优化冒泡版本1
    auto start = chrono::high_resolution_clock::now();
    BubbleSortOptimized(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  冒泡优化版本1:  " << duration.count() << " 微秒" << endl;
}

// 优化版本2：记录最后交换位置
void BubbleSortOptimized2(vector<int>& arr)
{
    int n = arr.size();
    int lastSwapIndex = n - 1;  // 记录最后一次交换的位置

    while (lastSwapIndex > 0) {
        int currentSwapIndex = 0;  // 当前轮最后交换的位置

        for (int j = 0; j < lastSwapIndex; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                currentSwapIndex = j;  // 更新最后交换位置
            }
        }

        // 最后交换位置之后的元素已经有序
        lastSwapIndex = currentSwapIndex;

        // 如果没有发生交换，则结束排序
        if (currentSwapIndex == 0) {
            break;
        }
    }
}

void BankBusinessOptimizeBubble2()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试优化冒泡版本1
    auto start = chrono::high_resolution_clock::now();
    BubbleSortOptimized2(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  冒泡优化版本2:  " << duration.count() << " 微秒" << endl;
}

// 鸡尾酒排序（双向冒泡排序）
void CocktailSort(vector<int>& arr)
{
    int n = arr.size();
    int left = 0;
    int right = n - 1;

    while (left < right) {
        bool swapped = false;

        // 从左到右，将最大元素移到右边
        for (int i = left; i < right; i++) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                swapped = true;
            }
        }
        right--;  // 右边界缩小

        // 如果没有发生交换，提前结束
        if (!swapped) {
            break;
        }

        swapped = false;

        // 从右到左，将最小元素移到左边
        for (int i = right; i > left; i--) {
            if (arr[i] < arr[i - 1]) {
                swap(arr[i], arr[i - 1]);
                swapped = true;
            }
        }
        left++;  // 左边界缩小

        // 如果没有发生交换，提前结束
        if (!swapped) {
            break;
        }
    }
}

void BankBusinessOptimizeCock()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试优化冒泡版本1
    auto start = chrono::high_resolution_clock::now();
    CocktailSort(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  双向冒泡排序:  " << duration.count() << " 微秒" << endl;
}

// 基础插入排序 - 升序
void InsertionSortBasic(vector<int>& arr)
{
    int n = arr.size();

    // 从第二个元素开始（第一个元素视为已排序）
    for (int i = 1; i < n; i++) {
        int key = arr[i];  // 当前要插入的元素
        int j = i - 1;      // 已排序部分的最后一个元素索引

        // 在已排序部分中找到合适位置
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];  // 元素后移
            j--;
        }

        // 插入元素到正确位置
        arr[j + 1] = key;
    }
}

void BankBusinessBaseInsertionSort()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试基础插入排序版本
    auto start = chrono::high_resolution_clock::now();
    InsertionSortBasic(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  基础插入排序:  " << duration.count() << " 微秒" << endl;
}

// 插入排序 - 降序
void InsertionSortDescending(vector<int>& arr)
{
    int n = arr.size();

    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        // 修改比较条件实现降序
        while (j >= 0 && arr[j] < key) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}

void BankBusinessBaseInsertionSortDec()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试基础插入排序降序版本
    auto start = chrono::high_resolution_clock::now();
    InsertionSortDescending(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  基础插入排序降序排序:  " << duration.count() << " 微秒" << endl;
}

// 使用二分查找优化插入位置查找
void InsertionSortBinarySearch(vector<int>& arr)
{
    int n = arr.size();

    for (int i = 1; i < n; i++) {
        int key = arr[i];

        // 使用二分查找找到插入位置
        int left = 0;
        int right = i - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;

            if (arr[mid] > key) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }

        // 将元素向后移动
        for (int j = i - 1; j >= left; j--) {
            arr[j + 1] = arr[j];
        }

        // 插入元素
        arr[left] = key;
    }
}

void BankBusinessInsertionSortBinary()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试二分插入排序版本
    auto start = chrono::high_resolution_clock::now();
    InsertionSortBinarySearch(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  二分插入排序:  " << duration.count() << " 微秒" << endl;
}

// 哨兵优化版本
void InsertionSortWithSentinel(vector<int>& arr)
{
    int n = arr.size();
    if (n <= 1) {
        return;
    }

    // 第一步：找到最小元素放在arr[0]作为哨兵
    int minIndex = 0;
    for (int i = 1; i < n; i++) {
        if (arr[i] < arr[minIndex]) {
            minIndex = i;
        }
    }
    swap(arr[0], arr[minIndex]);

    // 现在arr[0]是最小元素，可作为哨兵
    for (int i = 2; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        // 不需要检查j>=0，因为arr[0]是最小元素
        while (arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}

void BankBusinessInsertionSentinelSort()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试二分插入排序版本
    auto start = chrono::high_resolution_clock::now();
    InsertionSortWithSentinel(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  哨兵插入排序:  " << duration.count() << " 微秒" << endl;
}

// 泛型插入排序模板
template<typename T, typename Compare = std::less<T>>
void InsertionSortTemplate(vector<T>& arr, Compare comp = Compare())
{
    int n = arr.size();

    for (int i = 1; i < n; i++) {
        T key = arr[i];
        int j = i - 1;

        while (j >= 0 && comp(key, arr[j])) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}

// 使用示例
struct Person {
    string name;
    int age;

    bool operator<(const Person& other) const
    {
        return age < other.age;
    }
};

void BankBusinessinsertionTemplateSort()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<Person> people = {{"Alice", 25}, {"Bob", 20}, {"Charlie", 30}};

    // 测试泛型插入排序版本
    auto start = chrono::high_resolution_clock::now();
    InsertionSortTemplate(people);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  泛型插入排序:  " << duration.count() << " 微秒" << endl;
}

// 基础选择排序 - 升序
void SelectionSortBasic(vector<int>& arr)
{
    int n = arr.size();

    for (int i = 0; i < n - 1; i++) {
        // 假设当前位置是最小值
        int minIndex = i;

        // 在剩余部分中找到最小元素的索引
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }

        // 将找到的最小元素与当前位置交换
        if (minIndex != i) {
            swap(arr[i], arr[minIndex]);
        }
    }
}

void BankBusinessBaseselectionSort()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试基础选择排序版本
    auto start = chrono::high_resolution_clock::now();
    SelectionSortBasic(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  基础选择排序:  " << duration.count() << " 微秒" << endl;
}

// 选择排序 - 降序（找最大值）
void SelectionSortDescending(vector<int>& arr)
{
    int n = arr.size();

    for (int i = 0; i < n - 1; i++) {
        int maxIndex = i;

        for (int j = i + 1; j < n; j++) {
            if (arr[j] > arr[maxIndex]) {
                maxIndex = j;
            }
        }

        if (maxIndex != i) {
            swap(arr[i], arr[maxIndex]);
        }
    }
}

void BankBusinessBaseselectionDecSort()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试基础选择降序排序版本
    auto start = chrono::high_resolution_clock::now();
    SelectionSortDescending(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  基础选择降序排序:  " << duration.count() << " 微秒" << endl;
}

// 双向选择排序（鸡尾酒选择排序）
void BidirectionalSelectionSort(vector<int>& arr)
{
    int n = arr.size();
    int left = 0;
    int right = n - 1;

    while (left < right) {
        int minIndex = left;
        int maxIndex = left;

        // 在当前区间中找到最小和最大元素的索引
        for (int i = left + 1; i <= right; i++) {
            if (arr[i] < arr[minIndex]) {
                minIndex = i;
            }
            if (arr[i] > arr[maxIndex]) {
                maxIndex = i;
            }
        }

        // 将最小元素交换到left位置
        if (minIndex != left) {
            swap(arr[left], arr[minIndex]);

            // 如果最大元素原本在left位置，现在被移动到了minIndex位置
            if (maxIndex == left) {
                maxIndex = minIndex;
            }
        }

        // 将最大元素交换到right位置
        if (maxIndex != right) {
            swap(arr[right], arr[maxIndex]);
        }

        left++;
        right--;
    }
}

void BankBusinessBasebidirectionalSelectionSort()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试选择双向排序版本
    auto start = chrono::high_resolution_clock::now();
    BidirectionalSelectionSort(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  选择双向排序:  " << duration.count() << " 微秒" << endl;
}

// 记录位置最后再交换的版本
void SelectionSortDelayedSwap(vector<int>& arr)
{
    int n = arr.size();

    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        int minValue = arr[i];

        // 只记录最小值和位置，不立即交换
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < minValue) {
                minValue = arr[j];
                minIndex = j;
            }
        }

        // 最后进行一次交换
        if (minIndex != i) {
            // 将[i]到[minIndex-1]的元素向后移动
            for (int k = minIndex; k > i; k--) {
                arr[k] = arr[k - 1];
            }
            // 插入最小值
            arr[i] = minValue;
        }
    }
}

void BankBusinessBaseSelectionSortDelayedSwap()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 88};

    // 测试记录位置最后再交换选择排序版本
    auto start = chrono::high_resolution_clock::now();
    SelectionSortDelayedSwap(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  记录位置最后再交换选择排序:  " << duration.count() << " 微秒" << endl;
}

// 泛型选择排序模板
template<typename T, typename Compare = std::less<T>>
void SelectionSortTemplate(vector<T>& arr, Compare comp = Compare())
{
    int n = arr.size();

    for (int i = 0; i < n - 1; i++) {
        int targetIndex = i;

        for (int j = i + 1; j < n; j++) {
            if (comp(arr[j], arr[targetIndex])) {
                targetIndex = j;
            }
        }

        if (targetIndex != i) {
            swap(arr[i], arr[targetIndex]);
        }
    }
}

// 使用示例：按多个字段排序
struct Student {
    string name;
    int score;
    int age;

    // 按分数降序，年龄升序
    bool operator<(const Student& other) const
    {
        if (score != other.score) {
            return score > other.score;  // 分数高的在前
        }
        return age < other.age;  // 年龄小的在前
    }
};

void BankBusinessBaseSelectionSortTemplate()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<Person> people = {{"Alice", 25}, {"Bob", 20}, {"Charlie", 30}};

    // 测试选择模板排序版本
    auto start = chrono::high_resolution_clock::now();
    SelectionSortTemplate(people);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  选择模板排序:  " << duration.count() << " 微秒" << endl;
}

// 基础桶排序 - 假设输入是[0, maxValue]范围内的均匀分布
void BucketSortBasic(vector<int>& arr, int maxValue = 100)
{
    int n = arr.size();
    if (n <= 1) {
        return;
    }

    // 1. 创建桶，这里使用n个桶
    int bucketCount = n;
    vector<vector<int>> buckets(bucketCount);

    // 2. 计算每个元素应该放入哪个桶
    for (int i = 0; i < n; i++) {
        // 归一化到[0, n-1]范围
        int bucketIndex = arr[i] * bucketCount / (maxValue + 1);
        buckets[bucketIndex].push_back(arr[i]);
    }

    // 3. 对每个桶内部进行排序（这里使用插入排序）
    for (int i = 0; i < bucketCount; i++) {
        sort(buckets[i].begin(), buckets[i].end());
    }

    // 4. 合并所有桶
    int index = 0;
    for (int i = 0; i < bucketCount; i++) {
        for (int j = 0; j < buckets[i].size(); j++) {
            arr[index++] = buckets[i][j];
        }
    }
}

void BankBusinessBasebucketSort()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {29, 25, 3, 49, 9, 37, 21, 43};

    // 测试基础桶排序版本
    auto start = chrono::high_resolution_clock::now();
    BucketSortBasic(arr, FIFTY);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  基础桶排序:  " << duration.count() << " 微秒" << endl;
}

// 桶排序 - 针对[0, 1)范围内的浮点数
void BucketSortFloat(vector<double>& arr)
{
    int n = arr.size();
    if (n <= 1) {
        return;
    }

    // 1. 创建n个桶
    vector<vector<double>> buckets(n);

    // 2. 将每个元素放入对应的桶
    for (int i = 0; i < n; i++) {
        // 假设arr[i]在[0, 1)范围内
        int bucketIndex = n * arr[i];  // 乘以n得到桶索引
        buckets[bucketIndex].push_back(arr[i]);
    }

    // 3. 对每个桶排序
    for (int i = 0; i < n; i++) {
        sort(buckets[i].begin(), buckets[i].end());
    }

    // 4. 合并桶
    int index = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < buckets[i].size(); j++) {
            arr[index++] = buckets[i][j];
        }
    }
}

void BankBusinessBasebucketSortFloat()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
     vector<double> arr = {0.897, 0.565, 0.656, 0.1234, 0.665, 0.3434};

    // 测试基础浮点桶排序版本
    auto start = chrono::high_resolution_clock::now();
    BucketSortFloat(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  基础桶浮点排序:  " << duration.count() << " 微秒" << endl;
}

// 优化的桶排序，自动确定桶数量和使用合适的排序算法
class OptimizedBucketSort {
private:
    // 计算合适的桶数量
    int CalculateBucketCount(int n, int minVal, int maxVal)
    {
        if (n <= ONE_HUNDRED) {
            return TEN;  // 小数据量用较少桶
        }
        if (maxVal - minVal <= MAX_BUCKET_COUNT) {
            return FIFTY;  // 数据范围小
        }

        // 根据数据规模动态计算桶数量
        int bucketCount = sqrt(n);
        bucketCount = min(bucketCount, MAX_BUCKET_COUNT);  // 上限1000个桶
        bucketCount = max(bucketCount, MIN_BUCKET_COUNT);    // 下限10个桶

        return bucketCount;
    }

    // 根据桶大小选择排序算法
    void SortBucket(vector<int>& bucket)
    {
        int size = bucket.size();
        if (size <= SMALL_ARRAY_THRESHOLD) {
            // 小桶使用插入排序
            InsertionSort(bucket);
        } else if (size <= MEDIUM_ARRAY_THRESHOLD) {
            // 中等桶使用快速排序
            std::sort(bucket.begin(), bucket.end());
        } else {
            // 大桶使用标准库排序（通常是内省排序）
            std::sort(bucket.begin(), bucket.end());
        }
    }

    void InsertionSort(vector<int>& arr)
    {
        int n = arr.size();
        for (int i = 1; i < n; i++) {
            int key = arr[i];
            int j = i - 1;
            while (j >= 0 && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

public:
    void Sort(vector<int>& arr)
    {
        int n = arr.size();
        if (n <= 1) {
            return;
        }

        // 找到最小值和最大值
        int minVal = arr[0];
        int maxVal = arr[0];
        for (int i = 1; i < n; i++) {
            if (arr[i] < minVal) {
                minVal = arr[i];
            }
            if (arr[i] > maxVal) {
                maxVal = arr[i];
            }
        }

        // 如果所有元素相等，直接返回
        if (minVal == maxVal) {
            return;
        }

        // 计算桶数量
        int bucketCount = CalculateBucketCount(n, minVal, maxVal);

        // 创建桶
        vector<vector<int>> buckets(bucketCount);

        // 将元素分配到桶中
        double range = bucketCount == 0 ? 0 : static_cast<double>(maxVal - minVal + 1) / bucketCount;

        for (int i = 0; i < n; i++) {
            int bucketIndex = (arr[i] - minVal) / range;
            bucketIndex = min(bucketIndex, bucketCount - 1);  // 防止越界
            buckets[bucketIndex].push_back(arr[i]);
        }

        // 对每个桶排序
        for (int i = 0; i < bucketCount; i++) {
            if (!buckets[i].empty()) {
                SortBucket(buckets[i]);
            }
        }

        // 合并结果
        int index = 0;
        for (int i = 0; i < bucketCount; i++) {
            for (int val : buckets[i]) {
                arr[index++] = val;
            }
        }
    }
};

// 测试数据生成器
class TestDataGenerator {
public:
    // 生成均匀分布的整数
    static vector<int> generateUniformInt(int size, int minVal = 0, int maxVal = 1000)
    {
        vector<int> data(size);
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(minVal, maxVal);

        for (int i = 0; i < size; i++) {
            data[i] = dis(gen);
        }
        return data;
    }

    // 生成高斯分布的整数
    static vector<int> generateGaussianInt(int size, double mean = 500, double stddev = 100)
    {
        vector<int> data(size);
        random_device rd;
        mt19937 gen(rd());
        normal_distribution<> dis(mean, stddev);

        for (int i = 0; i < size; i++) {
            data[i] = max(ZERO, min(MAX_MATRIX_SIZE, (int)dis(gen)));
        }
        return data;
    }

    // 生成指数分布的整数
    static vector<int> generateExponentialInt(int size, double lambda = 0.01)
    {
        vector<int> data(size);
        random_device rd;
        mt19937 gen(rd());
        exponential_distribution<> dis(lambda);

        for (int i = 0; i < size; i++) {
            data[i] = min(TEST_SIZE_SMALL, (int)(dis(gen) * MEDIUM_ARRAY_THRESHOLD));
        }
        return data;
    }

    // 生成浮点数[0, 1)
    static vector<double> generateUniformDouble(int size)
    {
        vector<double> data(size);
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0.0, 1.0);

        for (int i = 0; i < size; i++) {
            data[i] = dis(gen);
        }
        return data;
    }

    // 生成随机字符串
    static vector<string> generateRandomStrings(int size, int maxLength = 10)
    {
        vector<string> data(size);
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> lenDis(1, maxLength);
        uniform_int_distribution<> charDis('a', 'z');

        for (int i = 0; i < size; i++) {
            int length = lenDis(gen);
            string s;
            for (int j = 0; j < length; j++) {
                s.push_back(charDis(gen));
            }
            data[i] = s;
        }
        return data;
    }

    // 生成包含负数的随机整数
    static vector<int> generateSignedInts(int size)
    {
        vector<int> data(size);
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(-FIVE_THOUSAND, FIVE_THOUSAND);

        for (int i = 0; i < size; i++) {
            data[i] = dis(gen);
        }
        return data;
    }

    // 生成固定位数的整数
    static vector<int> generateFixedDigitInts(int size, int digits)
    {
        vector<int> data(size);
        random_device rd;
        mt19937 gen(rd());

        int minVal = 1;
        for (int i = 1; i < digits; i++) {
            minVal *= TEN;
        }
        int maxVal = minVal * TEN - 1;

        uniform_int_distribution<> dis(minVal, maxVal);

        for (int i = 0; i < size; i++) {
            data[i] = dis(gen);
        }
        return data;
    }
};

void BankBusinessBasebucketSortOptimized()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> sizes = {TEST_SIZE_SMALL, TEST_SIZE_MEDIUM, TEST_SIZE_LARGE, TEST_SIZE_XLARGE};
    vector<pair<string, vector<int>>> testCases = {
        {"均匀分布", {}},
        {"高斯分布", {}},
        {"指数分布", {}}
    };

    OptimizedBucketSort bucketSorter;
    // 测试优化桶排序版本
    for (int size : sizes) {
        // 生成不同分布的数据
        vector<int> uniformData = TestDataGenerator::generateUniformInt(size);
        vector<int> gaussianData = TestDataGenerator::generateGaussianInt(size);
        vector<int> expData = TestDataGenerator::generateExponentialInt(size);

        vector<vector<int>> allData = {uniformData, gaussianData, expData};

        for (int i = 0; i < THREE; i++) {
            vector<int> data = allData[i];
            // 优化桶排序
            auto start = chrono::high_resolution_clock::now();
            bucketSorter.Sort(data);
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
            cout << i << "  优化桶排序:  " << duration.count() << " 微秒" << endl;
        }
    }
}

// 并行桶排序（使用多线程）
class ParallelBucketSort {
private:
    // 并行排序单个桶
    void ParallelSortBucket(vector<int>& bucket)
    {
        std::sort(bucket.begin(), bucket.end());
    }

public:
    void Sort(vector<int>& arr)
    {
        int n = arr.size();
        if (n <= 1) {
            return;
        }
        // 确定桶数量（根据CPU核心数优化）
        int bucketCount = min(n, static_cast<int>(thread::hardware_concurrency()) * 4);
        bucketCount = max(bucketCount, 4);  // 至少4个桶
        // 找到最小值和最大值
        int minVal = *min_element(arr.begin(), arr.end());
        int maxVal = *max_element(arr.begin(), arr.end());
        if (minVal == maxVal) {
            return;
        }
        // 创建桶
        vector<vector<int>> buckets(bucketCount);
        // 分配元素到桶中
        double range = 1.0;
        if (bucketCount != 0) {
            range = static_cast<double>(maxVal - minVal + 1) / bucketCount;
        }
        for (int val : arr) {
            int bucketIndex = 0;
            if (range != 0) {
                bucketIndex = (val - minVal) / range;
                bucketIndex = min(bucketIndex, bucketCount - 1);
                buckets[bucketIndex].push_back(val);
            }
        }
        // 并行排序每个桶
        vector<future<void>> futures;
        for (int i = 0; i < bucketCount; i++) {
            if (!buckets[i].empty()) {
                futures.push_back(async(launch::async,
                    [&buckets, i]() {
                        std::sort(buckets[i].begin(), buckets[i].end());
                    }));
            }
        }
        // 等待所有桶排序完成
        for (auto& f : futures) {
            f.wait();
        }
        // 合并结果
        int index = 0;
        for (int i = 0; i < bucketCount; i++) {
            for (int val : buckets[i]) {
                arr[index++] = val;
            }
        }
        return;
    }
};

// 泛型桶排序模板
template<typename T>
class GenericBucketSort {
private:
    // 哈希函数接口
    using HashFunction = function<int(const T&, int)>;

public:
    // 通用桶排序
    static void Sort(vector<T>& arr,
                     int bucketCount,
                     HashFunction hashFunc,
                     function<bool(const T&, const T&)> comp = less<T>())
    {
        int n = arr.size();
        if (n <= 1) {
            return;
        }

        // 创建桶
        vector<vector<T>> buckets(bucketCount);

        // 将元素分配到桶中
        for (const T& item : arr) {
            int bucketIndex = hashFunc(item, bucketCount);
            bucketIndex = max(0, min(bucketIndex, bucketCount - 1));
            buckets[bucketIndex].push_back(item);
        }

        // 对每个桶排序
        for (auto& bucket : buckets) {
            std::sort(bucket.begin(), bucket.end(), comp);
        }

        // 合并结果
        int index = 0;
        for (const auto& bucket : buckets) {
            for (const T& item : bucket) {
                arr[index++] = item;
            }
        }
    }
};

// 字符串桶排序示例
class StringBucketSorter {
public:
    static void SortByLength(vector<string>& arr)
    {
        // 按字符串长度分桶
        int maxLength = 0;
        for (const auto& s : arr) {
            maxLength = max(maxLength, (int)s.length());
        }

        // 创建桶（每个桶对应一个长度）
        vector<vector<string>> buckets(maxLength + 1);

        // 分配字符串到桶中
        for (const auto& s : arr) {
            buckets[s.length()].push_back(s);
        }

        // 对每个桶内的字符串按字典序排序
        for (auto& bucket : buckets) {
            sort(bucket.begin(), bucket.end());
        }

        // 合并结果
        int index = 0;
        for (const auto& bucket : buckets) {
            for (const auto& s : bucket) {
                arr[index++] = s;
            }
        }
    }

    static void SortByFirstChar(vector<string>& arr)
    {
        // 按首字母分桶（26个字母+其他）
        const int emptyBucketIndex = 26;
        const int emptyBucketIndexOther = 27;
        vector<vector<string>> buckets(emptyBucketIndexOther);  // 26个字母 + 其他字符

        for (const auto& s : arr) {
            if (s.empty()) {
                buckets[emptyBucketIndex].push_back(s);  // 空字符串放到最后一个桶
            } else {
                char firstChar = tolower(s[0]);
                if (firstChar >= 'a' && firstChar <= 'z') {
                    buckets[firstChar - 'a'].push_back(s);
                } else {
                    buckets[emptyBucketIndex].push_back(s);  // 非字母字符
                }
            }
        }

        // 对每个桶排序
        for (auto& bucket : buckets) {
            sort(bucket.begin(), bucket.end());
        }

        // 合并结果
        int index = 0;
        for (const auto& bucket : buckets) {
            for (const auto& s : bucket) {
                arr[index++] = s;
            }
        }
    }
};

// 获取数字的第d位（从个位开始，d=1表示个位）
int GetDigit(int num, int d)
    {
        for (int i = 1; i < d; i++) {
            num /= DECIMAL_BASE;
        }
        return num % DECIMAL_BASE;
    }

// 获取数组中最大数字的位数
int GetMaxDigits(const vector<int>& arr)
    {
    if (arr.empty()) {
        return 0;
    }

    int maxVal = *max_element(arr.begin(), arr.end());
    int digits = 0;

    while (maxVal > 0) {
        digits++;
        maxVal /= DECIMAL_BASE;
    }

    return max(1, digits);  // 至少1位
}

// LSD基数排序 - 十进制
void RadixSortLSD(vector<int>& arr)
{
    if (arr.size() <= 1) {
        return;
    }

    int maxDigits = GetMaxDigits(arr);
    int n = arr.size();

    // 临时数组
    vector<int> output(n);

    // 从个位开始，对每一位进行计数排序
    for (int digit = 1; digit <= maxDigits; digit++) {
        // 计数数组，0-9共10个数字
        vector<int> count(DECIMAL_BASE, 0);

        // 统计每个数字出现的次数
        for (int i = 0; i < n; i++) {
            int d = GetDigit(arr[i], digit);
            count[d]++;
        }

        // 将计数转换为位置索引
        for (int i = 1; i < DECIMAL_BASE; i++) {
            count[i] += count[i - 1];
        }

        // 从后向前遍历，保证稳定性
        for (int i = n - 1; i >= 0; i--) {
            int d = GetDigit(arr[i], digit);
            output[count[d] - 1] = arr[i];
            count[d]--;
        }

        // 复制回原数组
        arr = output;
    }
}

void BankBusinessBaseradixSort()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<int> arr = {29, 25, 3, 49, 9, 37, 21, 43};

    // 测试LSD基数排序版本
    auto start = chrono::high_resolution_clock::now();
    RadixSortLSD(arr);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  LSD基数排序:  " << duration.count() << " 微秒" << endl;
}

// 优化的LSD基数排序，支持2的幂作为基数
class OptimizedRadixSort {
private:
    // 获取数字在指定基数下的第k位
    int GetDigit(int num, int k, int radix)
    {
        int shiftAmount = k * static_cast<int>(log2(radix));
        return (num >> shiftAmount) & (radix - 1);
    }

    // 计算最大位数
    int GetMaxDigits(int maxVal, int radix)
    {
        int digits = 0;
        while (maxVal > 0 && radix != 0) {
            digits++;
            maxVal /= radix;
        }
        return max(1, digits);
    }

public:
    // 通用基数排序，radix必须是2的幂（2,4,8,16,32,64,128,256）
    void Sort(vector<int>& arr, int radix = DEFAULT_RADIX)
    {
        int n = arr.size();
        if (n <= 1) {
            return;
        }

        // 找到最大值
        int maxVal = *max_element(arr.begin(), arr.end());

        // 计算最大位数
        int maxDigits = GetMaxDigits(maxVal, radix);

        // 临时数组
        vector<int> output(n);

        // 对每一位进行计数排序
        for (int digit = 0; digit < maxDigits; digit++) {
            vector<int> count(radix, 0);

            // 统计频率
            for (int i = 0; i < n; i++) {
                int d = GetDigit(arr[i], digit, radix);
                count[d]++;
            }

            // 计算位置
            for (int i = 1; i < radix; i++) {
                count[i] += count[i - 1];
            }

            // 从后向前填充
            for (int i = n - 1; i >= 0; i--) {
                int d = GetDigit(arr[i], digit, radix);
                output[--count[d]] = arr[i];
            }

            // 交换数组
            arr.swap(output);
        }
    }
};

void BankBusinessradixSortOptimized()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    int size = TEST_SIZE_XLARGE;
    vector<int> data = TestDataGenerator::generateUniformInt(size, 0, TEST_MAX_VALUE);

    vector<int> radices = {2, 4, 8, 16, 32, 64, 128, DEFAULT_RADIX};
    for (int radix : radices) {
        OptimizedRadixSort sorter;
        // 测试基数优化排序版本
        auto start = chrono::high_resolution_clock::now();
        sorter.Sort(data, radix);
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        cout << radix << "  基数优化排序:  " << duration.count() << " 微秒" << endl;
    }
}

// MSD基数排序（递归实现）
class MSDRadixSort {
private:
    // 递归排序函数
    void MsdSort(vector<int>& arr, int left, int right, int digit, int maxDigit)
    {
        const int digitCount = 10;  // 0-9
        const int numBuckets = 11;
        if (left >= right || digit > maxDigit) {
            return;
        }

        int n = right - left + 1;
        if (n <= SMALL_ARRAY_THRESHOLD) {
            // 小数组使用插入排序
            InsertionSort(arr, left, right);
            return;
        }

        // 计数数组
        vector<int> count(digitCount + 1, 0);  // 0-9 + 一个额外位置

        // 临时数组
        vector<int> temp(n);

        // 统计频率（处理digit=0的情况）
        for (int i = left; i <= right; i++) {
            int d = (digit == 0) ? 0 : GetDigit(arr[i], digit);
            count[d + 1]++;  // +1为负数预留位置
        }

        // 计算起始位置
        for (int i = 1; i < numBuckets; i++) {
            count[i] += count[i - 1];
        }

        // 排序到临时数组
        for (int i = left; i <= right; i++) {
            int d = (digit == 0) ? 0 : GetDigit(arr[i], digit);
            temp[count[d]++] = arr[i];
        }

        // 复制回原数组
        for (int i = left; i <= right; i++) {
            arr[i] = temp[i - left];
        }

        // 递归排序每个桶
        int start = left;
        for (int i = 0; i < DECIMAL_BASE; i++) {
            int end = left + count[i] - 1;
            if (start <= end) {
                MsdSort(arr, start, end, digit - 1, maxDigit);
                start = end + 1;
            }
        }
    }

    void InsertionSort(vector<int>& arr, int left, int right)
    {
        for (int i = left + 1; i <= right; i++) {
            int key = arr[i];
            int j = i - 1;

            while (j >= left && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    int GetDigit(int num, int d)
    {
        for (int i = 1; i < d; i++) {
            num /= DECIMAL_BASE;
        }
        return num % DECIMAL_BASE;
    }

    int GetMaxDigits(const vector<int>& arr)
    {
        if (arr.empty()) {
            return 0;
        }

        int maxVal = *max_element(arr.begin(), arr.end());
        int digits = 0;

        while (maxVal > 0) {
            digits++;
            maxVal /= DECIMAL_BASE;
        }

        return max(1, digits);
    }

public:
    void Sort(vector<int>& arr)
    {
        if (arr.size() <= 1) {
            return;
        }

        int maxDigits = GetMaxDigits(arr);
        MsdSort(arr, 0, arr.size() - 1, maxDigits, maxDigits);
    }
};

void BankBusinessradixSortMSD()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    int size = TEST_SIZE_XLARGE;
    vector<int> data = TestDataGenerator::generateUniformInt(size, 0, TEST_MAX_VALUE);

    MSDRadixSort sorter;
    // 测试MSD基数排序版本
    auto start = chrono::high_resolution_clock::now();
    sorter.Sort(data);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  MSD基数排序:  " << duration.count() << " 微秒" << endl;
}

// 支持负数的基数排序
class SignedRadixSort {
private:
    // 分离正负数
    void SeparatePosNeg(vector<int>& arr)
    {
        int n = arr.size();
        int left = 0;
        int right = 0;
        right = n - 1;

        while (left <= right) {
            while (left <= right && arr[left] < 0) {
                left++;
            }
            while (left <= right && arr[right] >= 0) {
                right--;
            }

            if (left < right) {
                swap(arr[left], arr[right]);
            }
        }
    }

    // 对绝对值进行基数排序
    void RadixSortAbsolute(vector<int>& arr, int start, int end)
    {
        if (start >= end) {
            return;
        }

        int n = end - start + 1;
        vector<int> output(n);

        // 找到最大绝对值
        int maxAbs = 0;
        for (int i = start; i <= end; i++) {
            int absVal = abs(arr[i]);
            if (absVal > maxAbs) {
                maxAbs = absVal;
            }
        }

        // 计算最大位数
        int maxDigits = 0;
        while (maxAbs > 0) {
            maxDigits++;
            maxAbs /= DECIMAL_BASE;
        }
        maxDigits = max(1, maxDigits);

        // LSD基数排序
        for (int digit = 1; digit <= maxDigits; digit++) {
            vector<int> count(DECIMAL_BASE, 0);

            // 统计频率
            for (int i = start; i <= end; i++) {
                int num = abs(arr[i]);
                int d = GetDigit(num, digit);
                count[d]++;
            }

            // 计算位置
            for (int i = 1; i < DECIMAL_BASE; i++) {
                count[i] += count[i - 1];
            }

            // 从后向前填充
            for (int i = end; i >= start; i--) {
                int num = abs(arr[i]);
                int d = GetDigit(num, digit);
                output[--count[d]] = arr[i];
            }

            // 复制回原数组
            for (int i = start; i <= end; i++) {
                arr[i] = output[i - start];
            }
        }
    }

    int GetDigit(int num, int d)
    {
        for (int i = 1; i < d; i++) {
            num /= DECIMAL_BASE;
        }
        return num % DECIMAL_BASE;
    }

public:
    void Sort(vector<int>& arr)
    {
        if (arr.size() <= 1) {
            return;
        }

        // 分离正负数
        SeparatePosNeg(arr);

        // 找到正负数的分界点
        int negCount = 0;
        for (int num : arr) {
            if (num < 0) {
                negCount++;
            }
        }

        // 对负数部分按绝对值排序（降序，然后反转）
        if (negCount > 0) {
            RadixSortAbsolute(arr, 0, negCount - 1);
            // 反转负数部分
            reverse(arr.begin(), arr.begin() + negCount);
            // 将负数变回负号
            for (int i = 0; i < negCount; i++) {
                arr[i] = -arr[i];
            }
        }

        // 对正数部分排序
        if (negCount < arr.size()) {
            RadixSortAbsolute(arr, negCount, arr.size() - 1);
        }
    }
};

void BankBusinessradixSortSigned()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    int size = TEST_SIZE_XLARGE;
    vector<int> data = TestDataGenerator::generateUniformInt(size, 0, TEST_MAX_VALUE);

    SignedRadixSort sorter;
    // 测试支持负数的基数排序版本
    auto start = chrono::high_resolution_clock::now();
    sorter.Sort(data);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  支持负数的基数排序:  " << duration.count() << " 微秒" << endl;
}

// 字符串基数排序（LSD）
class StringRadixSort {
private:
    // 获取字符串的第k个字符，如果超出长度返回0
    char GetChar(const string& s, int k)
    {
        if (k < s.length()) {
            return s[k];
        }
        return 0;  // 空字符表示字符串结束
    }

    // 获取最大字符串长度
    int getMaxLength(const vector<string>& arr)
    {
        int maxLen = 0;
        for (const auto& s : arr) {
            maxLen = max(maxLen, (int)s.length());
        }
        return maxLen;
    }

public:
    // LSD字符串排序
    void SortLSD(vector<string>& arr)
    {
        if (arr.size() <= 1) {
            return;
        }

        int maxLen = getMaxLength(arr);
        int n = arr.size();

        // 从最后一个字符开始排序
        for (int pos = maxLen - 1; pos >= 0; pos--) {
            // 计数排序，ASCII码范围0-255
            vector<int> count(ASCII_CHAR_COUNT, 0);
            vector<string> output(n);

            // 统计频率
            for (int i = 0; i < n; i++) {
                char c = GetChar(arr[i], pos);
                count[c]++;
            }

            // 计算位置
            for (int i = 1; i < ASCII_CHAR_COUNT; i++) {
                count[i] += count[i - 1];
            }

            // 从后向前填充
            for (int i = n - 1; i >= 0; i--) {
                char c = GetChar(arr[i], pos);
                output[--count[c]] = arr[i];
            }

            // 更新数组
            arr = output;
        }
    }

    // MSD字符串排序
    void SortMSD(vector<string>& arr)
    {
        if (arr.size() <= 1) {
            return;
        }
        MsdSort(arr, 0, arr.size() - 1, 0);
    }

private:
    void MsdSort(vector<string>& arr, int low, int high, int depth)
    {
        if (low >= high) {
            return;
        }

        // 小数组使用插入排序
        if (high - low + 1 <= SMALL_ARRAY_THRESHOLD) {
            InsertionSort(arr, low, high, depth);
            return;
        }

        // 计数数组，+1用于空字符
        vector<int> count(ASCII_CHAR_COUNT + 2, 0);  // 0-255 + 2个额外位置

        // 临时数组
        vector<string> aux(high - low + 1);

        // 统计频率
        for (int i = low; i <= high; i++) {
            char c = GetChar(arr[i], depth);
            count[c + 2]++;  // +2为排序稳定性
        }

        // 计算起始位置
        for (int i = 1; i < ASCII_CHAR_COUNT + TWO; i++) {
            count[i] += count[i - 1];
        }

        // 排序到临时数组
        for (int i = low; i <= high; i++) {
            char c = GetChar(arr[i], depth);
            aux[count[c + 1]++] = arr[i];
        }

        // 复制回原数组
        for (int i = low; i <= high; i++) {
            arr[i] = aux[i - low];
        }

        // 递归排序每个桶
        for (int i = 0; i < ASCII_CHAR_COUNT; i++) {
            MsdSort(arr, low + count[i], low + count[i + 1] - 1, depth + 1);
        }
    }

    void InsertionSort(vector<string>& arr, int low, int high, int depth)
    {
        for (int i = low + 1; i <= high; i++) {
            for (int j = i; j > low; j--) {
                if (CompareStrings(arr[j], arr[j - 1], depth) < 0) {
                    swap(arr[j], arr[j - 1]);
                } else {
                    break;
                }
            }
        }
    }

    int CompareStrings(const string& a, const string& b, int depth)
    {
        int minLen = min(a.length(), b.length());
        for (int i = depth; i < minLen; i++) {
            if (a[i] != b[i]) {
                return a[i] - b[i];
            }
        }
        return a.length() - b.length();
    }
};

void BankBusinessradixSortString()
{
    usleep(SLEEP_TIME_US);
    LOGI("saving or withdraw VIP");

    // 简单示例
    vector<string> strings = {"apple", "banana", "grape", "apricot", "blueberry", "cherry"};

    StringRadixSort sorter;
    // 测试字符串基数排序版本
    auto start = chrono::high_resolution_clock::now();
    sorter.SortLSD(strings);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "  字符串基数排序:  " << duration.count() << " 微秒" << endl;
}

int SortClass::FfrtConcurrentQueue()
{
    // type传1，代表串行调度
    BankQueueSystem bankQueue(0, "Bank", DEFAULT_SORT_VERSION);

    auto task1 = bankQueue.Enter(BankBusiness, "customer1", ffrt_queue_priority_low, DEFAULT_QUEUE_PRIORITY);
    auto task2 = bankQueue.Enter(BankBusinessNew11, "customer2", ffrt_queue_priority_low, DEFAULT_QUEUE_PRIORITY);
    // VIP享受更优先的服务
    auto task3 = bankQueue.Enter(BankBusinessVIP, "customer3 vip", ffrt_queue_priority_high, DEFAULT_QUEUE_PRIORITY);

    auto task4 = bankQueue.Enter(BankBusinessBaseBubble, "customer4", ffrt_queue_priority_low, DEFAULT_QUEUE_PRIORITY);
    auto task5 = bankQueue.Enter(BankBusinessOptimizeBubble1, "customer5",
                                 ffrt_queue_priority_low, DEFAULT_QUEUE_PRIORITY);
    auto task6 = bankQueue.Enter(BankBusinessOptimizeBubble2, "customer6",
                                 ffrt_queue_priority_low, DEFAULT_QUEUE_PRIORITY);
    auto task7 = bankQueue.Enter(BankBusinessOptimizeCock, "customer7",
                                 ffrt_queue_priority_low, DEFAULT_QUEUE_PRIORITY);

    auto task8 = bankQueue.Enter(BankBusinessBaseInsertionSort, "customer8",
                                 ffrt_queue_priority_low, DEFAULT_QUEUE_PRIORITY);
    auto task9 = bankQueue.Enter(BankBusinessBaseInsertionSortDec, "customer9",
                                 ffrt_queue_priority_low, DEFAULT_QUEUE_PRIORITY);
    auto task10 = bankQueue.Enter(BankBusinessInsertionSortBinary, "customer10",
                                  ffrt_queue_priority_low, DEFAULT_QUEUE_PRIORITY);
    auto task11 = bankQueue.Enter(BankBusinessInsertionSentinelSort, "customer11",
                                  ffrt_queue_priority_low, DEFAULT_QUEUE_PRIORITY);
    auto task12 = bankQueue.Enter(BankBusinessinsertionTemplateSort, "customer12",
                                  ffrt_queue_priority_low, DEFAULT_QUEUE_PRIORITY);

    auto task13 = bankQueue.Enter(BankBusinessBaseselectionSort, "customer13",
                                  ffrt_queue_priority_low, DEFAULT_QUEUE_PRIORITY);
    // 等待所有的客户服务完成
    bankQueue.Wait(task1);
    bankQueue.Wait(task2);
    bankQueue.Wait(task3);
    bankQueue.Wait(task4);
    bankQueue.Wait(task5);
    bankQueue.Wait(task6);
    bankQueue.Wait(task7);
    bankQueue.Wait(task8);
    bankQueue.Wait(task9);
    bankQueue.Wait(task10);
    bankQueue.Wait(task11);
    bankQueue.Wait(task12);
    bankQueue.Wait(task13);
    LOGI("FfrtQueue results ");
    return RET_SUCCESS_6;
}
