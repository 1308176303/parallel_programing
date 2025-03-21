#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <ctime>

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

// 递归两两相加
double sum_recursive(double* arr, int start, int end) {
    if (start == end) {
        return arr[start];
    }
    if (start + 1 == end) {
        return arr[start] + arr[end];
    }
    
    int mid = start + (end - start) / 2;
    return sum_recursive(arr, start, mid) + sum_recursive(arr, mid + 1, end);
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
        
        // 递归算法对大规模数组可能溢出，先验证是否可行
        bool recursive_feasible = n <= 100000;  // 对大规模限制递归
        bool correct_recursive = false;
        
        if (recursive_feasible) {
            try {
                double recursive_result = sum_recursive(arr, 0, n - 1);
                correct_recursive = abs(naive_result - recursive_result) < 1e-10;
            } catch (...) {
                recursive_feasible = false;
            }
        }
        
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
        double total_time_recursive = 0.0;
        if (recursive_feasible) {
            for (int t = 0; t < actual_test_count; t++) {
                double start_time = get_time();
                volatile double res = sum_recursive(arr, 0, n - 1);
                total_time_recursive += (get_time() - start_time);
                
                // 输出进度
                if ((t+1) % 10 == 0 || t == actual_test_count-1) {
                    cout << "  递归算法进度: " << t+1 << "/" << actual_test_count << endl;
                }
            }
        }
        
        // 计算加速比
        double speedup_two_way = total_time_naive / total_time_two_way;
        double speedup_recursive = recursive_feasible ? (total_time_naive / total_time_recursive) : 0.0;
        
        string correctness = "";
        if (correct_two_way && (recursive_feasible ? correct_recursive : true)) {
            correctness = "正确";
        } else {
            correctness = "错误";
            if (!correct_two_way) correctness += "-两路";
            if (recursive_feasible && !correct_recursive) correctness += "-递归";
        }
        
        // 输出结果到控制台
        cout << n << "\t" 
             << fixed << setprecision(6) << total_time_naive << "\t\t" 
             << total_time_two_way << "\t\t";
        
        if (recursive_feasible) {
            cout << total_time_recursive << "\t\t";
        } else {
            cout << "N/A\t\t";
        }
        
        cout << setprecision(2) << speedup_two_way << "x\t\t";
        
        if (recursive_feasible) {
            cout << speedup_recursive << "x\t\t";
        } else {
            cout << "N/A\t\t";
        }
        
        cout << correctness << endl;
        
        // 写入CSV文件
        out_file << n << "," 
                 << fixed << setprecision(6) << total_time_naive << "," 
                 << total_time_two_way << ",";
        
        if (recursive_feasible) {
            out_file << total_time_recursive;
        } else {
            out_file << "N/A";
        }
        
        out_file << "," << setprecision(3) << speedup_two_way << ",";
        
        if (recursive_feasible) {
            out_file << speedup_recursive;
        } else {
            out_file << "N/A";
        }
        
        out_file << "," << correctness << endl;
        
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
    
    // 测试的数组大小 - 减少最大规模以加快测试
    int sizes[] = {100, 500, 1000,5000, 10000, 50000,100000, 250000, 500000,1000000, 2500000, 5000000,10000000, 20000000, 30000000};
    int sizes_count = sizeof(sizes) / sizeof(sizes[0]);
    
    int test_count = 50;  // 每个规模测试50次
    
    cout << "========== 数组求和算法性能测试 ==========" << endl;
    cout << "从小规模到大规模递增测试，每个规模测试" << test_count << "次" << endl;
    
    // 基础算法测试
    test_basic_sum(sizes, sizes_count, test_count, "jichu_sum.csv");
    
    // 进阶算法测试
    test_advanced_sum(sizes, sizes_count, test_count, "jinjie_sum.csv");
    
    return 0;
}