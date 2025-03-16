#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>

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
        for (int j = 0; j < n; j++) {
            result[j] += matrix[i][j] * vector[i];
        }
    }
}

// 测试矩阵向量内积算法
void test(int n, int repeat) {
    cout << "\n测试矩阵列与向量内积计算 (矩阵大小: " << n << "x" << n << ", 重复次数: " << repeat << ")" << endl;
    
    // 分配内存
    double** matrix = new double*[n];
    for (int i = 0; i < n; i++) {
        matrix[i] = new double[n];
    }
    double* vector = new double[n];
    double* resulta = new double[n];
    double* resultb = new double[n];
    
    // 生成测试数据
    generate_data(matrix, vector, n);
    
    // 测试平凡算法
    double start_time = get_time();
    for (int r = 0; r < repeat; r++) {
        mula(matrix, vector, resulta, n);
    }
    double timea = get_time() - start_time;
    
    // 测试优化算法
    start_time = get_time();
    for (int r = 0; r < repeat; r++) {
        mulb(matrix, vector, resultb, n);
    }
    double timeb = get_time() - start_time;
    
    // 验证结果正确性
    bool correct = true;
    for (int i = 0; i < n; i++) {
        if (abs(resulta[i] - resultb[i]) > 1e-10) {
            correct = false;
            break;
        }
    }
    
    // 输出结果
    cout << "平凡算法用时: " << fixed << setprecision(9) << timea << " 秒" << endl;
    cout << "优化算法用时: " << fixed << setprecision(9) << timeb << " 秒" << endl;
    cout << "加速比: " << fixed << setprecision(2) << timea / timeb << "x" << endl;
    cout << "结果正确性: " << (correct ? "正确" : "错误") << endl;
    
    // 释放内存
    for (int i = 0; i < n; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
    delete[] vector;
    delete[] resulta;
    delete[] resultb;
}

int main() {
    const int n = 50;  // 固定数组大小为7000
    
    cout << "========== 矩阵列与向量内积计算 ==========" << endl;
    
    // 测试矩阵向量内积算法
    test(n, 100);  
    
    return 0;
}