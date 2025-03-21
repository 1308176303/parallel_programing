#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <cmath>

using namespace std;

// 高精度计时函数
double get_time() {
    return static_cast<double>(chrono::duration_cast<chrono::nanoseconds>(
        chrono::high_resolution_clock::now().time_since_epoch()).count()) / 1.0e9;
}

// 生成随机矩阵和向量
void generate_data(double** matrix, double* vector, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // 使用固定值便于验证正确性
            matrix[i][j] = (i * n + j) % 10 + 1.0;
        }
        vector[i] = i % 5 + 1.0;
    }
}

// 方法a: 逐列访问元素的平凡算法
void mula(double** matrix, double* vector, double* result, int n) {
    for (int j = 0; j < n; j++) {  // 遍历每一列
        double sum = 0.0;
        for (int i = 0; i < n; i++) {  // 计算内积
            sum += matrix[i][j] * vector[i];
        }
        result[j] = sum;
    }
}

// 方法b: cache优化算法
void mulb(double** matrix, double* vector, double* result, int n) {
    // 初始化结果数组
    for (int j = 0; j < n; j++) {
        result[j] = 0.0;
    }
    
    // 按行访问矩阵元素，利用空间局部性
    for (int i = 0; i < n; i++) {
        double vi = vector[i];  // 减少内存访问
        for (int j = 0; j < n; j++) {
            result[j] += matrix[i][j] * vi;
        }
    }
}

// 方法c: 4路循环展开
void mulc(double** matrix, double* vector, double* result, int n) {
    // 初始化结果数组
    for (int j = 0; j < n; j++) {
        result[j] = 0.0;
    }
    
    // 按行访问矩阵元素，利用空间局部性，同时展开循环
    int i = 0;
    for (; i < n-3; i += 4) {
        double v0 = vector[i];
        double v1 = vector[i+1];
        double v2 = vector[i+2];
        double v3 = vector[i+3];
        
        for (int j = 0; j < n; j++) {
            result[j] += matrix[i][j] * v0 +
                         matrix[i+1][j] * v1 +
                         matrix[i+2][j] * v2 +
                         matrix[i+3][j] * v3;
        }
    }
    
    // 处理剩余元素
    for (; i < n; i++) {
        double vi = vector[i];
        for (int j = 0; j < n; j++) {
            result[j] += matrix[i][j] * vi;
        }
    }
}

// 方法d: 8路循环展开
void muld(double** matrix, double* vector, double* result, int n) {
    // 初始化结果数组
    for (int j = 0; j < n; j++) {
        result[j] = 0.0;
    }
    
    // 按行访问矩阵元素，利用空间局部性，同时展开循环
    int i = 0;
    for (; i < n-7; i += 8) {
        double v0 = vector[i];
        double v1 = vector[i+1];
        double v2 = vector[i+2];
        double v3 = vector[i+3];
        double v4 = vector[i+4];
        double v5 = vector[i+5];
        double v6 = vector[i+6];
        double v7 = vector[i+7];
        
        for (int j = 0; j < n; j++) {
            result[j] += matrix[i][j] * v0 +
                         matrix[i+1][j] * v1 +
                         matrix[i+2][j] * v2 +
                         matrix[i+3][j] * v3 +
                         matrix[i+4][j] * v4 +
                         matrix[i+5][j] * v5 +
                         matrix[i+6][j] * v6 +
                         matrix[i+7][j] * v7;
        }
    }
    
    // 处理剩余元素
    for (; i < n; i++) {
        double vi = vector[i];
        for (int j = 0; j < n; j++) {
            result[j] += matrix[i][j] * vi;
        }
    }
}

// 测试基础矩阵乘法：平凡算法与Cache优化对比
void test_basic_mul(int* sizes, int sizes_count, int test_count, const char* output_file) {
    ofstream out_file(output_file);
    if (!out_file.is_open()) {
        cout << "无法创建文件: " << output_file << endl;
        return;
    }
    
    // 写入CSV文件头
    out_file << "矩阵大小,平凡算法(秒),Cache优化(秒),加速比,结果正确性" << endl;
    
    // 控制台表头
    cout << "\n基础矩阵乘法算法性能比较 (每规模测试" << test_count << "次):" << endl;
    cout << "规模\t平凡算法(秒)\tCache优化(秒)\t加速比\t结果正确性" << endl;
    cout << "------\t-----------\t-----------\t------\t----------" << endl;
    
    for (int i = 0; i < sizes_count; i++) {
        int n = sizes[i];
        cout << "测试矩阵大小: " << n << "x" << n << " (" << test_count << "次)" << endl;
        
        // 分配内存
        double** matrix = new double*[n];
        for (int j = 0; j < n; j++) {
            matrix[j] = new double[n];
        }
        double* vector = new double[n];
        double* result_naive = new double[n];
        double* result_cache = new double[n];
        
        // 生成测试数据
        generate_data(matrix, vector, n);
        
        // 调整迭代次数，对大规模数据减少迭代
        int actual_test_count = test_count;
        if (n > 1000) actual_test_count = (test_count > 20) ? 20 : test_count;
        if (n > 5000) actual_test_count = (test_count > 10) ? 10 : test_count;
        
        cout << "  调整后测试次数: " << actual_test_count << endl;
        
        // 验证结果是否正确（只需验证一次）
        mula(matrix, vector, result_naive, n);
        mulb(matrix, vector, result_cache, n);
        
        bool correct = true;
        for (int j = 0; j < n; j++) {
            if (abs(result_naive[j] - result_cache[j]) > 1e-10) {
                correct = false;
                break;
            }
        }
        
        // 测试平凡算法 - 累计所有测试时间
        double total_time_naive = 0.0;
        for (int t = 0; t < actual_test_count; t++) {
            double start_time = get_time();
            mula(matrix, vector, result_naive, n);
            total_time_naive += (get_time() - start_time);
            
            // 输出进度
            if ((t+1) % 10 == 0 || t == actual_test_count-1) {
                cout << "  平凡算法进度: " << t+1 << "/" << actual_test_count << endl;
            }
        }
        
        // 测试Cache优化算法 - 累计所有测试时间
        double total_time_cache = 0.0;
        for (int t = 0; t < actual_test_count; t++) {
            double start_time = get_time();
            mulb(matrix, vector, result_cache, n);
            total_time_cache += (get_time() - start_time);
            
            // 输出进度
            if ((t+1) % 10 == 0 || t == actual_test_count-1) {
                cout << "  Cache优化算法进度: " << t+1 << "/" << actual_test_count << endl;
            }
        }
        
        // 计算加速比
        double speedup = total_time_naive / total_time_cache;
        
        // 输出结果到控制台
        cout << n << "\t" 
             << fixed << setprecision(6) << total_time_naive << "\t\t" 
             << total_time_cache << "\t\t"
             << setprecision(2) << speedup << "x\t"
             << (correct ? "正确" : "错误")
             << endl;
        
        // 写入CSV文件
        out_file << n << "," 
                 << fixed << setprecision(6) << total_time_naive << "," 
                 << total_time_cache << ","
                 << setprecision(3) << speedup << ","
                 << (correct ? "正确" : "错误")
                 << endl;
        
        // 释放内存
        for (int j = 0; j < n; j++) {
            delete[] matrix[j];
        }
        delete[] matrix;
        delete[] vector;
        delete[] result_naive;
        delete[] result_cache;
    }
    
    out_file.close();
    cout << "基础矩阵乘法测试结果已保存到: " << output_file << endl;
}

// 测试进阶矩阵乘法：平凡算法与循环展开算法对比
void test_advanced_mul(int* sizes, int sizes_count, int test_count, const char* output_file) {
    ofstream out_file(output_file);
    if (!out_file.is_open()) {
        cout << "无法创建文件: " << output_file << endl;
        return;
    }
    
    // 写入CSV文件头
    out_file << "矩阵大小,平凡算法(秒),4路展开(秒),8路展开(秒),4路展开加速比,8路展开加速比,结果正确性" << endl;
    
    // 控制台表头
    cout << "\n进阶矩阵乘法算法性能比较 (每规模测试" << test_count << "次):" << endl;
    cout << "规模\t平凡算法(秒)\t4路展开(秒)\t8路展开(秒)\t4路加速比\t8路加速比\t结果正确性" << endl;
    cout << "------\t-----------\t-----------\t-----------\t----------\t----------\t----------" << endl;
    
    for (int i = 0; i < sizes_count; i++) {
        int n = sizes[i];
        cout << "测试矩阵大小: " << n << "x" << n << " (" << test_count << "次)" << endl;
        
        // 分配内存
        double** matrix = new double*[n];
        for (int j = 0; j < n; j++) {
            matrix[j] = new double[n];
        }
        double* vector = new double[n];
        double* result_naive = new double[n];
        double* result_unroll4 = new double[n];
        double* result_unroll8 = new double[n];
        
        // 生成测试数据
        generate_data(matrix, vector, n);
        
        // 调整迭代次数，对大规模数据减少迭代
        int actual_test_count = test_count;
        if (n > 1000) actual_test_count = (test_count > 20) ? 20 : test_count;
        if (n > 5000) actual_test_count = (test_count > 10) ? 10 : test_count;
        
        cout << "  调整后测试次数: " << actual_test_count << endl;
        
        // 验证结果是否正确（只需验证一次）
        mula(matrix, vector, result_naive, n);
        mulc(matrix, vector, result_unroll4, n);
        muld(matrix, vector, result_unroll8, n);
        
        bool correct4 = true, correct8 = true;
        for (int j = 0; j < n; j++) {
            if (abs(result_naive[j] - result_unroll4[j]) > 1e-10) {
                correct4 = false;
                break;
            }
            if (abs(result_naive[j] - result_unroll8[j]) > 1e-10) {
                correct8 = false;
                break;
            }
        }
        
        // 测试平凡算法 - 累计所有测试时间
        double total_time_naive = 0.0;
        for (int t = 0; t < actual_test_count; t++) {
            double start_time = get_time();
            mula(matrix, vector, result_naive, n);
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
            mulc(matrix, vector, result_unroll4, n);
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
            muld(matrix, vector, result_unroll8, n);
            total_time_unroll8 += (get_time() - start_time);
            
            // 输出进度
            if ((t+1) % 10 == 0 || t == actual_test_count-1) {
                cout << "  8路展开算法进度: " << t+1 << "/" << actual_test_count << endl;
            }
        }
        
        // 计算加速比
        double speedup4 = total_time_naive / total_time_unroll4;
        double speedup8 = total_time_naive / total_time_unroll8;
        
        // 输出结果到控制台
        cout << n << "\t" 
             << fixed << setprecision(6) << total_time_naive << "\t\t" 
             << total_time_unroll4 << "\t\t"
             << total_time_unroll8 << "\t\t"
             << setprecision(2) << speedup4 << "x\t\t"
             << speedup8 << "x\t\t"
             << (correct4 && correct8 ? "正确" : "错误")
             << endl;
        
        // 写入CSV文件
        out_file << n << "," 
                 << fixed << setprecision(6) << total_time_naive << "," 
                 << total_time_unroll4 << ","
                 << total_time_unroll8 << ","
                 << setprecision(3) << speedup4 << ","
                 << speedup8 << ","
                 << (correct4 && correct8 ? "正确" : "错误")
                 << endl;
        
        // 释放内存
        for (int j = 0; j < n; j++) {
            delete[] matrix[j];
        }
        delete[] matrix;
        delete[] vector;
        delete[] result_naive;
        delete[] result_unroll4;
        delete[] result_unroll8;
    }
    
    out_file.close();
    cout << "进阶矩阵乘法测试结果已保存到: " << output_file << endl;
}

int main() {
    srand(time(NULL));
    
    // 测试的矩阵大小 - 从小到大
    int sizes[] = {50, 100, 200,300, 500, 700,1000, 1500, 2000,2500, 3000, 4000,5000, 6000, 7000};
    int sizes_count = sizeof(sizes) / sizeof(sizes[0]);
    
    int test_count = 50;  // 每个规模测试50次
    
    cout << "========== 矩阵向量乘法性能优化测试 ==========" << endl;
    cout << "从小规模到大规模递增测试，每个规模测试" << test_count << "次" << endl;
    
    // 测试基础算法：平凡算法vs缓存优化
    test_basic_mul(sizes, sizes_count, test_count, "jichu_matrix.csv");
    
    // 测试进阶算法：平凡算法vs循环展开
    test_advanced_mul(sizes, sizes_count, test_count, "jinjie_matrix.csv");
    
    return 0;
}