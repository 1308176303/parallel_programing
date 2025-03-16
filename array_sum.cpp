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

// 生成随机数组
void generate_array(double* arr, int n) {
    for (int i = 0; i < n; i++) {
        // 使用固定值便于验证正确性
        arr[i] = i % 10 + 1.0;
    }
}

// 方法a: 逐个累加的平凡算法（链式）
double suma(double* arr, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

// 方法b: 两路链式累加
double sumb(double* arr, int n) {
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

// 方法c: 递归两两相加
double sumc(double* arr, int start, int end) {
    if (start == end) {
        return arr[start];
    }
    if (start + 1 == end) {
        return arr[start] + arr[end];
    }
    
    int mid = start + (end - start) / 2;
    return sumc(arr, start, mid) + sumc(arr, mid + 1, end);
}

// 测试数组求和算法
void test_sum(int n, int repeat) {
    cout << "\n测试n个数的求和 (数组大小: " << n << ", 重复次数: " << repeat << ")" << endl;
    
    // 分配内存
    double* arr = new double[n];
    
    // 生成测试数据
    generate_array(arr, n);
    
    // 测试平凡算法
    double resulta = 0.0;
    double start_time = get_time();
    for (int r = 0; r < repeat; r++) {
        resulta = suma(arr, n);
    }
    double timea = get_time() - start_time;
    
    // 测试两路链式累加
    double resultb = 0.0;
    start_time = get_time();
    for (int r = 0; r < repeat; r++) {
        resultb = sumb(arr, n);
    }
    double timeb = get_time() - start_time;
    
    // 测试递归两两相加
    double resultc = 0.0;
    start_time = get_time();
    for (int r = 0; r < repeat; r++) {
        resultc = sumc(arr, 0, n - 1);
    }
    double timec = get_time() - start_time;
    
    // 输出结果
    cout << "平凡算法用时: " << fixed << setprecision(6) << timea << " 秒" << endl;
    cout << "两路链式累加用时: " << fixed << setprecision(6) << timeb << " 秒" << endl;
    cout << "递归两两相加用时: " << fixed << setprecision(6) << timec << " 秒" << endl;
    cout << "两路链式加速比: " << fixed << setprecision(2) << timea / timeb << "x" << endl;
    cout << "递归两两加速比: " << fixed << setprecision(2) << timea / timec << "x" << endl;
    cout << "结果正确性 (两路链式): " << (abs(resulta - resultb) < 1e-10 ? "正确" : "错误") << endl;
    cout << "结果正确性 (递归两两): " << (abs(resulta - resultc) < 1e-10 ? "正确" : "错误") << endl;
    
    // 释放内存
    delete[] arr;
}

int main() {
    const int n = 50;  // 固定数组大小为7000
    
    cout << "========== n个数的求和算法优化 ==========" << endl;
    
    // 测试数组求和算法
    test_sum(n, 100);  // 由于求和运算较快，保留一定重复次数
    
    return 0;
}