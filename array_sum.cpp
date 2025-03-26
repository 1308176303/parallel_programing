#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <algorithm>

using namespace std;

// 高精度计时函数，返回秒
double get_time() {
    return static_cast<double>(chrono::duration_cast<chrono::nanoseconds>(
        chrono::high_resolution_clock::now().time_since_epoch()).count()) / 1.0e9;
}

// 生成测试数据
void generate_data(double* arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = i % 10 + 1.0;  // 使用固定模式方便验证
    }
}

// 平凡求和算法
double sum_naive(double* arr, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

// 两路链式求和算法
double sum_two_way(double* arr, int n) {
    double sum1 = 0.0;
    double sum2 = 0.0;
    int i;
    
    // 两路并行累加
    for (i = 0; i < n - 1; i += 2) {
        sum1 += arr[i];
        sum2 += arr[i + 1];
    }
    
    // 处理剩余元素
    if (i < n) {
        sum1 += arr[i];
    }
    
    return sum1 + sum2;
}

// 原地递归规约算法 - 直接修改输入数组
double sum_reduction(double* arr, int n) {
    int m = n;
    while (m > 1) {
        int half = m / 2;
        for (int i = 0; i < half; i++) {
            arr[i] += arr[i + half]; 
        }
        // 处理奇数
        if (m % 2 == 1) {
            arr[0] += arr[m-1];
        }
        m = half;
    }
    return arr[0];
}

// 4路循环展开
double sum_unroll4(double* arr, int n) {
    double sum = 0.0;
    int i = 0;
    
    // 4路循环展开
    for (; i + 3 < n; i += 4) {
        sum += arr[i] + arr[i+1] + arr[i+2] + arr[i+3];
    }
    
    // 处理剩余元素
    for (; i < n; i++) {
        sum += arr[i];
    }
    
    return sum;
}

// 8路循环展开
double sum_unroll8(double* arr, int n) {
    double sum = 0.0;
    int i = 0;
    
    // 8路循环展开
    for (; i + 7 < n; i += 8) {
        sum += arr[i] + arr[i+1] + arr[i+2] + arr[i+3] +
               arr[i+4] + arr[i+5] + arr[i+6] + arr[i+7];
    }
    
    // 处理剩余元素
    for (; i < n; i++) {
        sum += arr[i];
    }
    
    return sum;
}

// 测试基础求和算法
void test_basic_sum(int* sizes, int sizes_count, int test_count, const char* output_file) {
    ofstream out_file(output_file);
    if (!out_file.is_open()) {
        cout << "无法创建文件: " << output_file << endl;
        return;
    }
    
    // 写入CSV文件头
    out_file << "数组大小,平凡算法(秒),两路链式(秒),递归(秒),两路链式加速比,递归加速比,结果正确性" << endl;
    
    // 控制台表头
    cout << "\n基础求和算法性能比较 (每规模测试" << test_count << "次):" << endl;
    cout << "规模\t平凡算法(秒)\t两路链式(秒)\t递归(秒)\t两路链式加速比\t递归加速比\t结果正确性" << endl;
    cout << "-------\t-----------\t-----------\t--------\t--------------\t-----------\t----------" << endl;
    
    for (int i = 0; i < sizes_count; i++) {
        int n = sizes[i];
        cout << "测试数组大小: " << n << " (" << test_count << "次)" << endl;
        
        // 动态分配测试数组
        double* arr = new double[n];
        generate_data(arr, n);
        
        // 调整迭代次数，对大规模数据减少迭代
        int actual_test_count = test_count;
        if (n > 1000000) actual_test_count = (test_count > 10) ? 10 : test_count;
        if (n > 10000000) actual_test_count = (test_count > 5) ? 5 : test_count;
        
        cout << "  调整后测试次数: " << actual_test_count << endl;
        
        // 先验证结果正确性（只需验证一次）
        double naive_result = sum_naive(arr, n);
        double two_way_result = sum_two_way(arr, n);
        
        bool correct_two_way = abs(naive_result - two_way_result) < 1e-10;
        
        // 验证规约算法正确性 - 为规约算法创建数组副本
        bool correct_recursive = false;
        double* arr_copy = new double[n];
        memcpy(arr_copy, arr, n * sizeof(double));
        double recursive_result = sum_reduction(arr_copy, n);
        correct_recursive = abs(naive_result - recursive_result) < 1e-10;
        delete[] arr_copy;
        
        // 测试平凡算法 - 累计所有测试时间
        double total_time_naive = 0.0;
        for (int t = 0; t < actual_test_count; t++) {
            double start_time = get_time();
            volatile double res = sum_naive(arr, n);
            total_time_naive += (get_time() - start_time);
            
            // 输出进度
            if ((t+1) % 10 == 0 || t == actual_test_count-1) {
                cout << "  平凡算法进度: " << t+1 << "/" << actual_test_count << endl;
            }
        }
        
        // 测试两路链式算法 - 累计所有测试时间
        double total_time_two_way = 0.0;
        for (int t = 0; t < actual_test_count; t++) {
            double start_time = get_time();
            volatile double res = sum_two_way(arr, n);
            total_time_two_way += (get_time() - start_time);
            
            // 输出进度
            if ((t+1) % 10 == 0 || t == actual_test_count-1) {
                cout << "  两路链式算法进度: " << t+1 << "/" << actual_test_count << endl;
            }
        }
        
        // 测试递归算法 - 累计所有测试时间
        // 为每次测试创建数组副本
        double total_time_recursive = 0.0;
        double* arr_temp = new double[n]; // 用于递归算法的临时数组
        
        for (int t = 0; t < actual_test_count; t++) {
            // 为每次测试创建数组副本
            memcpy(arr_temp, arr, n * sizeof(double));
            
            double start_time = get_time();
            volatile double res = sum_reduction(arr_temp, n);
            total_time_recursive += (get_time() - start_time);
            
            // 输出进度
            if ((t+1) % 10 == 0 || t == actual_test_count-1) {
                cout << "  递归算法进度: " << t+1 << "/" << actual_test_count << endl;
            }
        }
        
        delete[] arr_temp; // 释放临时数组
        
        // 计算加速比
        double speedup_two_way = total_time_naive / total_time_two_way;
        double speedup_recursive = total_time_naive / total_time_recursive;
        
        string correctness = "";
        if (correct_two_way && correct_recursive) {
            correctness = "正确";
        } else {
            correctness = "错误";
            if (!correct_two_way) correctness += "-两路";
            if (!correct_recursive) correctness += "-递归";
        }
        
        // 输出结果到控制台
        cout << n << "\t" 
             << fixed << setprecision(6) << total_time_naive << "\t\t" 
             << total_time_two_way << "\t\t"
             << total_time_recursive << "\t\t"
             << setprecision(2) << speedup_two_way << "x\t\t"
             << speedup_recursive << "x\t\t"
             << correctness << endl;
        
        // 写入CSV文件
        out_file << n << "," 
                 << fixed << setprecision(6) << total_time_naive << "," 
                 << total_time_two_way << ","
                 << total_time_recursive << ","
                 << setprecision(3) << speedup_two_way << ","
                 << speedup_recursive << ","
                 << correctness << endl;
        
        // 释放内存
        delete[] arr;
    }
    
    out_file.close();
    cout << "基础算法测试结果已保存到: " << output_file << endl;
}

// 测试进阶求和算法
void test_advanced_sum(int* sizes, int sizes_count, int test_count, const char* output_file) {
    ofstream out_file(output_file);
    if (!out_file.is_open()) {
        cout << "无法创建文件: " << output_file << endl;
        return;
    }
    
    // 写入CSV文件头
    out_file << "数组大小,平凡算法(秒),4路展开(秒),8路展开(秒),4路展开加速比,8路展开加速比,结果正确性" << endl;
    
    // 控制台表头
    cout << "\n进阶求和算法性能比较 (每规模测试" << test_count << "次):" << endl;
    cout << "规模\t平凡算法(秒)\t4路展开(秒)\t8路展开(秒)\t4路展开加速比\t8路展开加速比\t结果正确性" << endl;
    cout << "-------\t-----------\t-----------\t-----------\t--------------\t--------------\t----------" << endl;
    
    for (int i = 0; i < sizes_count; i++) {
        int n = sizes[i];
        cout << "测试数组大小: " << n << " (" << test_count << "次)" << endl;
        
        // 动态分配测试数组
        double* arr = new double[n];
        generate_data(arr, n);
        
        // 调整迭代次数，对大规模数据减少迭代
        int actual_test_count = test_count;
        if (n > 1000000) actual_test_count = (test_count > 10) ? 10 : test_count;
        if (n > 10000000) actual_test_count = (test_count > 5) ? 5 : test_count;
        
        cout << "  调整后测试次数: " << actual_test_count << endl;
        
        // 先验证结果正确性（只需验证一次）
        double naive_result = sum_naive(arr, n);
        double unroll4_result = sum_unroll4(arr, n);
        double unroll8_result = sum_unroll8(arr, n);
        
        bool correct_unroll4 = abs(naive_result - unroll4_result) < 1e-10;
        bool correct_unroll8 = abs(naive_result - unroll8_result) < 1e-10;
        
        // 测试平凡算法 - 累计所有测试时间
        double total_time_naive = 0.0;
        for (int t = 0; t < actual_test_count; t++) {
            double start_time = get_time();
            volatile double res = sum_naive(arr, n);
            total_time_naive += (get_time() - start_time);
            
            // 输出进度
            if ((t+1) % 10 == 0 || t == actual_test_count-1) {
                cout << "  平凡算法进度: " << t+1 << "/" << actual_test_count << endl;
            }
        }
        
        // 测试4路循环展开 - 累计所有测试时间
        double total_time_unroll4 = 0.0;
        for (int t = 0; t < actual_test_count; t++) {
            double start_time = get_time();
            volatile double res = sum_unroll4(arr, n);
            total_time_unroll4 += (get_time() - start_time);
            
            // 输出进度
            if ((t+1) % 10 == 0 || t == actual_test_count-1) {
                cout << "  4路展开算法进度: " << t+1 << "/" << actual_test_count << endl;
            }
        }
        
        // 测试8路循环展开 - 累计所有测试时间
        double total_time_unroll8 = 0.0;
        for (int t = 0; t < actual_test_count; t++) {
            double start_time = get_time();
            volatile double res = sum_unroll8(arr, n);
            total_time_unroll8 += (get_time() - start_time);
            
            // 输出进度
            if ((t+1) % 10 == 0 || t == actual_test_count-1) {
                cout << "  8路展开算法进度: " << t+1 << "/" << actual_test_count << endl;
            }
        }
        
        // 计算加速比
        double speedup_unroll4 = total_time_naive / total_time_unroll4;
        double speedup_unroll8 = total_time_naive / total_time_unroll8;
        
        string correctness = "";
        if (correct_unroll4 && correct_unroll8) {
            correctness = "正确";
        } else {
            correctness = "错误";
            if (!correct_unroll4) correctness += "-4路";
            if (!correct_unroll8) correctness += "-8路";
        }
        
        // 输出结果到控制台
        cout << n << "\t" 
             << fixed << setprecision(6) << total_time_naive << "\t\t" 
             << total_time_unroll4 << "\t\t"
             << total_time_unroll8 << "\t\t"
             << setprecision(2) << speedup_unroll4 << "x\t\t"
             << speedup_unroll8 << "x\t\t"
             << correctness << endl;
        
        // 写入CSV文件
        out_file << n << "," 
                 << fixed << setprecision(6) << total_time_naive << "," 
                 << total_time_unroll4 << ","
                 << total_time_unroll8 << ","
                 << setprecision(3) << speedup_unroll4 << ","
                 << speedup_unroll8 << ","
                 << correctness << endl;
        
        // 释放内存
        delete[] arr;
    }
    
    out_file.close();
    cout << "进阶算法测试结果已保存到: " << output_file << endl;
}

int main() {
    srand(time(NULL));
    
    // 根据不同规模范围设置2的幂次方测试规模
    vector<int> test_sizes;

    // 小规模：2^7(128) 到 2^13(8192)
    for (int i = 7; i <= 13; i++) {
        test_sizes.push_back(1 << i);  // 2的幂
    }
    
    // L1缓存临界点：512KB约等于64K个双精度数
    // L1缓存临界点附近(60K-70K)细粒度采样
    int l1_start = 60000;
    int l1_end = 70000;
    int l1_step = 1000;
    
    // 找到最接近L1临界点的2的幂
    int l1_pow2 = 1 << 16;  // 2^16 = 65536
    test_sizes.push_back(l1_pow2);
    
    // 添加L1临界点周围的额外采样点
    for (int i = l1_start; i <= l1_end; i += l1_step) {
        if (i != l1_pow2) {  // 避免重复添加2的幂点
            test_sizes.push_back(i);
        }
    }
    
    // 中等规模：2^17(131072) 到 2^19(524288)
    for (int i = 17; i <= 19; i++) {
        test_sizes.push_back(1 << i);
    }
    
    // L2缓存临界点：8MB约等于1M个双精度数
    // L2缓存临界点附近(950K-1050K)细粒度采样
    int l2_start = 950000;
    int l2_end = 1050000;
    int l2_step = 10000;
    
    // 找到最接近L2临界点的2的幂
    int l2_pow2 = 1 << 20;  // 2^20 = 1048576
    test_sizes.push_back(l2_pow2);
    
    // 添加L2临界点周围的额外采样点
    for (int i = l2_start; i <= l2_end; i += l2_step) {
        if (i != l2_pow2) {  // 避免重复添加2的幂点
            test_sizes.push_back(i);
        }
    }
    
    // 大规模：2^21(2097152) 到 2^22(4194304)
    for (int i = 21; i <= 22; i++) {
        test_sizes.push_back(1 << i);
    }
    
    // L3缓存临界点：16MB约等于2M个双精度数
    // L3缓存临界点附近(1.95M-2.05M)细粒度采样
    int l3_start = 1950000;
    int l3_end = 2050000;
    int l3_step = 10000;
    
    // 添加L3临界点周围的额外采样点
    for (int i = l3_start; i <= l3_end; i += l3_step) {
        if (i != (1 << 21)) {  // 避免重复添加2的幂点
            test_sizes.push_back(i);
        }
    }
    
    // 超大规模：2^23(8388608) 到 2^25(33554432)
    for (int i = 23; i <= 25; i++) {
        test_sizes.push_back(1 << i);
    }
    
    // 将vector转换为数组
    int sizes_count = test_sizes.size();
    int* sizes = new int[sizes_count];
    for (int i = 0; i < sizes_count; i++) {
        sizes[i] = test_sizes[i];
    }
    
    // 对规模排序，确保按大小顺序测试
    sort(sizes, sizes + sizes_count);
    
    int test_count = 50;  // 每个规模测试50次
    
    cout << "========== 数组求和算法性能测试 ==========" << endl;
    cout << "使用2的幂次方规模测试，并在缓存临界点周围进行细粒度采样" << endl;
    cout << "共" << sizes_count << "个规模，每个规模测试" << test_count << "次" << endl;
    cout << "L1缓存临界点(~64K), L2缓存临界点(~1M), L3缓存临界点(~2M)" << endl;
    
    // 基础算法测试
    test_basic_sum(sizes, sizes_count, test_count, "jichu_sum.csv");
    
    // 进阶算法测试
    test_advanced_sum(sizes, sizes_count, test_count, "jinjie_sum.csv");
    
    delete[] sizes;
    return 0;
}