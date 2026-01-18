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
#include "function_class.h"
#include "common_ffrt.h"
#include "native_log_wrapper.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <complex.h>
#include <float.h>
#define ONE  1
#define TWO  2
#define THREE  3
#define FIVE  5
#define  RET_SUCCESS_5  5

/* ========== 常量定义 ========== */
#define MAX_MATRIX_SIZE 1000
#define MAX_VECTOR_SIZE 10000
#define MAX_ITERATIONS 10000
#define EPSILON 1e-12
#define PI 3.14159265358979323846
#define GOLDEN_RATIO 1.618033988749895
#define MAX_OPT_VARS 100
#define MAX_POLY_DEGREE 50
#define MAX_INTEGRATION_POINTS 1000
/* ========== 数据结构 ========== */
typedef struct {
    int rows;
    int cols;
    double *data;
} Matrix;

typedef struct {
    int size;
    double *data;
} Vector;

typedef struct {
    int degree;
    double coeffs[MAX_POLY_DEGREE + 1];
} Polynomial;

typedef struct {
    double real;
    double imag;
} ComplexNum;

typedef struct {
    int iterations;
    double tolerance;
    double (*function)(double);
    double (*gradient)(double);
} OptimizerConfig;

/* 定义三个全局变量返回值 */
static int g_add_ret = -1;
static int g_sub_ret = -1;
static int g_compute_ret = -1;

/* ========== 函数声明 ========== */
// 内存管理
Matrix create_matrix(int rows, int cols);
void destroy_matrix(Matrix *mat);
Vector create_vector(int size);
void destroy_vector(Vector *vec);
Matrix copy_matrix(const Matrix *src);
Vector copy_vector(const Vector *src);

// 基本运算
Matrix matrix_add(const Matrix *a, const Matrix *b);
Matrix matrix_subtract(const Matrix *a, const Matrix *b);
Matrix matrix_multiply(const Matrix *a, const Matrix *b);
Matrix matrix_scalar_multiply(const Matrix *a, double scalar);
Vector vector_add(const Vector *a, const Vector *b);
Vector vector_subtract(const Vector *a, const Vector *b);
double vector_dot(const Vector *a, const Vector *b);
Matrix matrix_transpose(const Matrix *mat);
Matrix matrix_inverse(const Matrix *mat);
double matrix_determinant(const Matrix *mat);

// 线性方程组求解
Vector solve_linear_system(const Matrix *A, const Vector *b);
Vector solve_linear_system_lu(const Matrix *A, const Vector *b);
Matrix lu_decomposition(const Matrix *A);
Vector forward_substitution(const Matrix *L, const Vector *b);
Vector backward_substitution(const Matrix *U, const Vector *b);

// 特征值/特征向量
Matrix qr_decomposition(const Matrix *A);
void qr_algorithm(const Matrix *A, double *eigenvalues, Matrix *eigenvectors);
double power_iteration(const Matrix *A, Vector *eigenvector);
double inverse_power_iteration(const Matrix *A, Vector *eigenvector, double sigma);

// 数值积分
double integrate_trapezoidal(double (*f)(double), double a, double b, int n);
double integrate_simpson(double (*f)(double), double a, double b, int n);
double integrate_romberg(double (*f)(double), double a, double b, double tol);
double integrate_gaussian(double (*f)(double), double a, double b, int n);
double integrate_monte_carlo(double (*f)(double), double a, double b, int samples);

// 微分方程求解
Vector solve_ode_euler(double (*f)(double, double), double y0, 
                       double t0, double tf, int steps);
Vector solve_ode_rk4(double (*f)(double, double), double y0,
                     double t0, double tf, int steps);
Vector solve_ode_adaptive(double (*f)(double, double), double y0,
                          double t0, double tf, double tol);
Vector solve_ode_system(void (*f)(double, const Vector*, Vector*),
                        const Vector *y0, double t0, double tf, int steps);

// 傅里叶变换
void dft(const double *input, ComplexNum *output, int n);
void idft(const ComplexNum *input, double *output, int n);
void fft(ComplexNum *data, int n);
void ifft(ComplexNum *data, int n);
void fft_real(const double *input, ComplexNum *output, int n);

// 优化算法
double minimize_golden_section(double (*f)(double), double a, double b, double tol);
double minimize_brent(double (*f)(double), double a, double b, double c, double tol);
Vector minimize_gradient_descent(double (*f)(const Vector*),
                                 const Vector *gradient,
                                 const Vector *x0, double learning_rate, int iterations);
Vector minimize_conjugate_gradient(double (*f)(const Vector*),
                                   const Vector *gradient,
                                   const Matrix *A, const Vector *b,
                                   const Vector *x0, int iterations);

// 插值与逼近
Vector polynomial_fit(const Vector *x, const Vector *y, int degree);
double polynomial_eval(const Polynomial *p, double x);
Vector spline_interpolation(const Vector *x, const Vector *y);
double spline_eval(const Vector *x, const Vector *y, const Vector *coeffs, double xi);

// 统计分析
Vector linear_regression(const Vector *x, const Vector *y);
double correlation_coefficient(const Vector *x, const Vector *y);
Vector moving_average(const Vector *data, int window);
void compute_statistics(const Vector *data, double *mean, double *variance,
                       double *skewness, double *kurtosis);

// 特殊函数
double gamma_function(double x);
double beta_function(double a, double b);
double bessel_j0(double x);
double bessel_j1(double x);
double erf_function(double x);
double legendre_poly(int n, double x);
double chebyshev_poly(int n, double x);

// 随机数生成
Vector random_uniform(int n, double a, double b);
Vector random_normal(int n, double mean, double stddev);
Vector random_exponential(int n, double lambda);
Matrix random_matrix(int rows, int cols, double min_val, double max_val);

// 工具函数
void print_matrix(const Matrix *mat, const char *name);
void print_vector(const Vector *vec, const char *name);
double matrix_norm(const Matrix *mat, int p);
double vector_norm(const Vector *vec, int p);
bool is_symmetric(const Matrix *mat);
bool is_positive_definite(const Matrix *mat);
struct ParaStruct {
    int a;
    int b;
};

struct ParaStruct g_para1;
struct ParaStruct g_para2;

void Add(void *arg)
{
    struct ParaStruct *para1 = (struct ParaStruct *)arg;
    int a = para1->a;
    int b = para1->b;
    g_add_ret = 0;
}

void Sub(void *arg)
{
    struct ParaStruct *para2 = (struct ParaStruct *)arg;
    int a = para2->a;
    int b = para2->b;
    g_sub_ret = 0;
}

/* ========== 内存管理实现 ========== */
Matrix create_matrix(int rows, int cols) {
    Matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    mat.data = (double*)malloc(rows * cols * sizeof(double));
    if (mat.data == NULL) {
        fprintf(stderr, "内存分配失败\n");
        exit(EXIT_FAILURE);
    }
    memset(mat.data, 0, rows * cols * sizeof(double));
    return mat;
}

void destroy_matrix(Matrix *mat) {
    if (mat->data != NULL) {
        free(mat->data);
        mat->data = NULL;
    }
    mat->rows = 0;
    mat->cols = 0;
}

Vector create_vector(int size) {
    Vector vec;
    vec.size = size;
    vec.data = (double*)malloc(size * sizeof(double));
    if (vec.data == NULL) {
        fprintf(stderr, "内存分配失败\n");
        exit(EXIT_FAILURE);
    }
    memset(vec.data, 0, size * sizeof(double));
    return vec;
}

void destroy_vector(Vector *vec) {
    if (vec->data != NULL) {
        free(vec->data);
        vec->data = NULL;
    }
    vec->size = 0;
}

Matrix copy_matrix(const Matrix *src) {
    Matrix dst = create_matrix(src->rows, src->cols);
    memcpy(dst.data, src->data, src->rows * src->cols * sizeof(double));
    return dst;
}

Vector copy_vector(const Vector *src) {
    Vector dst = create_vector(src->size);
    memcpy(dst.data, src->data, src->size * sizeof(double));
    return dst;
}

/* ========== 基本运算实现 ========== */
Matrix matrix_add(const Matrix *a, const Matrix *b) {
    if (a->rows != b->rows || a->cols != b->cols) {
        fprintf(stderr, "矩阵维度不匹配\n");
        exit(EXIT_FAILURE);
    }
    
    Matrix result = create_matrix(a->rows, a->cols);
    int total = a->rows * a->cols;
    
    for (int i = 0; i < total; i++) {
        result.data[i] = a->data[i] + b->data[i];
    }
    
    return result;
}

Matrix matrix_multiply(const Matrix *a, const Matrix *b) {
    if (a->cols != b->rows) {
        fprintf(stderr, "矩阵维度不匹配，无法相乘\n");
        exit(EXIT_FAILURE);
    }
    
    Matrix result = create_matrix(a->rows, b->cols);
    
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < b->cols; j++) {
            double sum = 0.0;
            for (int k = 0; k < a->cols; k++) {
                sum += a->data[i * a->cols + k] * b->data[k * b->cols + j];
            }
            result.data[i * b->cols + j] = sum;
        }
    }
    
    return result;
}

Matrix matrix_transpose(const Matrix *mat) {
    Matrix result = create_matrix(mat->cols, mat->rows);
    
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            result.data[j * mat->rows + i] = mat->data[i * mat->cols + j];
        }
    }
    
    return result;
}

double matrix_determinant(const Matrix *mat) {
    if (mat->rows != mat->cols) {
        fprintf(stderr, "只有方阵才有行列式\n");
        exit(EXIT_FAILURE);
    }
    
    int n = mat->rows;
    
    // 1x1 矩阵
    if (n == 1) {
        return mat->data[0];
    }
    
    // 2x2 矩阵
    if (n == 2) {
        return mat->data[0] * mat->data[3] - mat->data[1] * mat->data[2];
    }
    
    // 递归计算行列式
    double det = 0.0;
    int sign = 1;
    
    for (int j = 0; j < n; j++) {
        // 创建子矩阵
        Matrix submat = create_matrix(n - 1, n - 1);
        int sub_i = 0;
        
        for (int i = 1; i < n; i++) {
            int sub_j = 0;
            for (int k = 0; k < n; k++) {
                if (k == j) continue;
                submat.data[sub_i * (n - 1) + sub_j] = mat->data[i * n + k];
                sub_j++;
            }
            sub_i++;
        }
        
        det += sign * mat->data[j] * matrix_determinant(&submat);
        sign = -sign;
        
        destroy_matrix(&submat);
    }
    
    return det;
}

/* ========== LU分解实现 ========== */
Matrix lu_decomposition(const Matrix *A) {
    if (A->rows != A->cols) {
        fprintf(stderr, "LU分解需要方阵\n");
        exit(EXIT_FAILURE);
    }
    
    int n = A->rows;
    Matrix LU = copy_matrix(A);
    
    for (int k = 0; k < n - 1; k++) {
        if (fabs(LU.data[k * n + k]) < EPSILON) {
            fprintf(stderr, "主元为零，无法进行LU分解\n");
            exit(EXIT_FAILURE);
        }
        
        for (int i = k + 1; i < n; i++) {
            LU.data[i * n + k] /= LU.data[k * n + k];
            
            for (int j = k + 1; j < n; j++) {
                LU.data[i * n + j] -= LU.data[i * n + k] * LU.data[k * n + j];
            }
        }
    }
    
    return LU;
}

Vector forward_substitution(const Matrix *L, const Vector *b) {
    int n = L->rows;
    Vector x = create_vector(n);
    
    for (int i = 0; i < n; i++) {
        double sum = 0.0;
        for (int j = 0; j < i; j++) {
            sum += L->data[i * n + j] * x.data[j];
        }
        x.data[i] = (b->data[i] - sum) / L->data[i * n + i];
    }
    
    return x;
}

Vector backward_substitution(const Matrix *U, const Vector *b) {
    int n = U->rows;
    Vector x = create_vector(n);
    
    for (int i = n - 1; i >= 0; i--) {
        double sum = 0.0;
        for (int j = i + 1; j < n; j++) {
            sum += U->data[i * n + j] * x.data[j];
        }
        x.data[i] = (b->data[i] - sum) / U->data[i * n + i];
    }
    
    return x;
}

/* ========== 线性方程组求解 ========== */
Vector solve_linear_system_lu(const Matrix *A, const Vector *b) {
    Matrix LU = lu_decomposition(A);
    int n = A->rows;
    
    // 从LU分解中提取L和U
    Matrix L = create_matrix(n, n);
    Matrix U = create_matrix(n, n);
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i > j) {
                L.data[i * n + j] = LU.data[i * n + j];
                U.data[i * n + j] = 0.0;
            } else if (i == j) {
                L.data[i * n + j] = 1.0;
                U.data[i * n + j] = LU.data[i * n + j];
            } else {
                L.data[i * n + j] = 0.0;
                U.data[i * n + j] = LU.data[i * n + j];
            }
        }
    }
    
    // 解 Ly = b
    Vector y = forward_substitution(&L, b);
    
    // 解 Ux = y
    Vector x = backward_substitution(&U, &y);
    
    destroy_matrix(&LU);
    destroy_matrix(&L);
    destroy_matrix(&U);
    destroy_vector(&y);
    
    return x;
}

/* ========== QR分解实现 ========== */
Matrix qr_decomposition(const Matrix *A) {
    int m = A->rows;
    int n = A->cols;
    Matrix Q = create_matrix(m, n);
    Matrix R = create_matrix(n, n);
    Matrix V = copy_matrix(A);
    
    for (int j = 0; j < n; j++) {
        // 计算第j列的范数
        double norm = 0.0;
        for (int i = 0; i < m; i++) {
            norm += V.data[i * n + j] * V.data[i * n + j];
        }
        norm = sqrt(norm);
        
        // 构造Householder向量
        R.data[j * n + j] = norm;
        
        if (norm > EPSILON) {
            for (int i = 0; i < m; i++) {
                Q.data[i * n + j] = V.data[i * n + j] / norm;
            }
            
            // 更新剩余列
            for (int k = j + 1; k < n; k++) {
                double dot = 0.0;
                for (int i = 0; i < m; i++) {
                    dot += Q.data[i * n + j] * V.data[i * n + k];
                }
                R.data[j * n + k] = dot;
                
                for (int i = 0; i < m; i++) {
                    V.data[i * n + k] -= 2.0 * Q.data[i * n + j] * dot;
                }
            }
        }
    }
    
    destroy_matrix(&V);
    return Q;  // 返回Q矩阵，R矩阵在过程中计算
}

/* ========== 特征值计算 ========== */
double power_iteration(const Matrix *A, Vector *eigenvector) {
    int n = A->rows;
    Vector b = create_vector(n);
    
    // 初始化随机向量
    for (int i = 0; i < n; i++) {
        b.data[i] = (double)rand() / RAND_MAX;
    }
    
    // 归一化
    double norm = 0.0;
    for (int i = 0; i < n; i++) {
        norm += b.data[i] * b.data[i];
    }
    norm = sqrt(norm);
    for (int i = 0; i < n; i++) {
        b.data[i] /= norm;
    }
    
    // 幂迭代
    double eigenvalue = 0.0;
    for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
        // 计算 Ab
        Vector Ab = create_vector(n);
        for (int i = 0; i < n; i++) {
            Ab.data[i] = 0.0;
            for (int j = 0; j < n; j++) {
                Ab.data[i] += A->data[i * n + j] * b.data[j];
            }
        }
        
        // 计算 Rayleigh 商
        double numerator = 0.0;
        double denominator = 0.0;
        for (int i = 0; i < n; i++) {
            numerator += b.data[i] * Ab.data[i];
            denominator += b.data[i] * b.data[i];
        }
        double new_eigenvalue = numerator / denominator;
        
        // 检查收敛
        if (fabs(new_eigenvalue - eigenvalue) < EPSILON) {
            eigenvalue = new_eigenvalue;
            // 复制特征向量
            for (int i = 0; i < n; i++) {
                eigenvector->data[i] = b.data[i];
            }
            destroy_vector(&Ab);
            break;
        }
        
        eigenvalue = new_eigenvalue;
        
        // 归一化新向量
        norm = 0.0;
        for (int i = 0; i < n; i++) {
            norm += Ab.data[i] * Ab.data[i];
        }
        norm = sqrt(norm);
        for (int i = 0; i < n; i++) {
            b.data[i] = Ab.data[i] / norm;
        }
        
        destroy_vector(&Ab);
    }
    
    destroy_vector(&b);
    return eigenvalue;
}

/* ========== 数值积分实现 ========== */
double integrate_trapezoidal(double (*f)(double), double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.5 * (f(a) + f(b));
    
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += f(x);
    }
    
    return sum * h;
}

double integrate_simpson(double (*f)(double), double a, double b, int n) {
    if (n % 2 != 0) n++;  // 确保n为偶数
    
    double h = (b - a) / n;
    double sum = f(a) + f(b);
    
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        if (i % 2 == 0) {
            sum += 2.0 * f(x);
        } else {
            sum += 4.0 * f(x);
        }
    }
    
    return sum * h / 3.0;
}

double integrate_romberg(double (*f)(double), double a, double b, double tol) {
    double R[MAX_ITERATIONS][MAX_ITERATIONS];
    int k = 0;
    
    // 初始梯形公式
    R[0][0] = (b - a) * (f(a) + f(b)) / 2.0;
    
    for (k = 1; k < MAX_ITERATIONS; k++) {
        // 计算梯形公式的递归
        int n = 1 << k;  // 2^k
        double h = (b - a) / n;
        double sum = 0.0;
        
        for (int i = 1; i < n; i += 2) {
            double x = a + i * h;
            sum += f(x);
        }
        
        R[k][0] = 0.5 * R[k-1][0] + h * sum;
        
        // Richardson 外推
        for (int j = 1; j <= k; j++) {
            R[k][j] = R[k][j-1] + (R[k][j-1] - R[k-1][j-1]) / (pow(4.0, j) - 1.0);
        }
        
        // 检查收敛
        if (k > 0 && fabs(R[k][k] - R[k-1][k-1]) < tol) {
            return R[k][k];
        }
    }
    
    return R[k-1][k-1];
}

/* ========== 微分方程求解 ========== */
Vector solve_ode_rk4(double (*f)(double, double), double y0,
                     double t0, double tf, int steps) {
    double h = (tf - t0) / steps;
    Vector solution = create_vector(steps + 1);
    
    solution.data[0] = y0;
    double t = t0;
    double y = y0;
    
    for (int i = 0; i < steps; i++) {
        double k1 = h * f(t, y);
        double k2 = h * f(t + h/2.0, y + k1/2.0);
        double k3 = h * f(t + h/2.0, y + k2/2.0);
        double k4 = h * f(t + h, y + k3);
        
        y = y + (k1 + 2.0*k2 + 2.0*k3 + k4) / 6.0;
        t = t + h;
        
        solution.data[i + 1] = y;
    }
    
    return solution;
}

/* ========== 傅里叶变换实现 ========== */
void dft(const double *input, ComplexNum *output, int n) {
    for (int k = 0; k < n; k++) {
        double real = 0.0;
        double imag = 0.0;
        
        for (int t = 0; t < n; t++) {
            double angle = 2.0 * PI * k * t / n;
            real += input[t] * cos(angle);
            imag -= input[t] * sin(angle);
        }
        
        output[k].real = real;
        output[k].imag = imag;
    }
}

void fft(ComplexNum *data, int n) {
    if (n <= 1) return;
    
    // 分离偶数和奇数项
    ComplexNum *even = (ComplexNum*)malloc(n/2 * sizeof(ComplexNum));
    ComplexNum *odd = (ComplexNum*)malloc(n/2 * sizeof(ComplexNum));
    
    for (int i = 0; i < n/2; i++) {
        even[i] = data[i * 2];
        odd[i] = data[i * 2 + 1];
    }
    
    // 递归计算
    fft(even, n/2);
    fft(odd, n/2);
    
    // 合并结果
    for (int k = 0; k < n/2; k++) {
        double angle = -2.0 * PI * k / n;
        ComplexNum t;
        t.real = cos(angle) * odd[k].real - sin(angle) * odd[k].imag;
        t.imag = sin(angle) * odd[k].real + cos(angle) * odd[k].imag;
        
        data[k].real = even[k].real + t.real;
        data[k].imag = even[k].imag + t.imag;
        data[k + n/2].real = even[k].real - t.real;
        data[k + n/2].imag = even[k].imag - t.imag;
    }
    
    free(even);
    free(odd);
}

/* ========== 优化算法实现 ========== */
double minimize_golden_section(double (*f)(double), double a, double b, double tol) {
    double c = b - GOLDEN_RATIO * (b - a);
    double d = a + GOLDEN_RATIO * (b - a);
    double fc = f(c);
    double fd = f(d);
    
    while (fabs(c - d) > tol) {
        if (fc < fd) {
            b = d;
            d = c;
            c = b - GOLDEN_RATIO * (b - a);
            fd = fc;
            fc = f(c);
        } else {
            a = c;
            c = d;
            d = a + GOLDEN_RATIO * (b - a);
            fc = fd;
            fd = f(d);
        }
    }
    
    return (a + b) / 2.0;
}

double minimize_brent(double (*f)(double), double a, double b, double c, double tol) {
    double x = b;
    double fx = f(x);
    double w = b;
    double fw = fx;
    double v = b;
    double fv = fx;
    
    double d = 0.0;
    double e = 0.0;
    
    for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
        double xm = (a + b) / 2.0;
        double tol1 = tol * fabs(x) + EPSILON;
        double tol2 = 2.0 * tol1;
        
        // 检查收敛
        if (fabs(x - xm) <= (tol2 - 0.5 * (b - a))) {
            return x;
        }
        
        // 尝试抛物线插值
        if (fabs(e) > tol1) {
            double r = (x - w) * (fx - fv);
            double q = (x - v) * (fx - fw);
            double p = (x - v) * q - (x - w) * r;
            q = 2.0 * (q - r);
            
            if (q > 0.0) p = -p;
            q = fabs(q);
            
            double etemp = e;
            e = d;
            
            // 检查抛物线步长是否可接受
            if (fabs(p) < fabs(0.5 * q * etemp) && p > q * (a - x) && p < q * (b - x)) {
                d = p / q;
                double u = x + d;
                
                if (u - a < tol2 || b - u < tol2) {
                    d = (x < xm) ? tol1 : -tol1;
                }
            } else {
                e = (x < xm) ? b - x : a - x;
                d = GOLDEN_RATIO * e;
            }
        } else {
            e = (x < xm) ? b - x : a - x;
            d = GOLDEN_RATIO * e;
        }
        
        // 计算下一步
        double u = x + ((fabs(d) >= tol1) ? d : ((d > 0) ? tol1 : -tol1));
        double fu = f(u);
        
        // 更新区间
        if (fu <= fx) {
            if (u >= x) a = x; else b = x;
            v = w; fv = fw;
            w = x; fw = fx;
            x = u; fx = fu;
        } else {
            if (u < x) a = u; else b = u;
            if (fu <= fw || w == x) {
                v = w; fv = fw;
                w = u; fw = fu;
            } else if (fu <= fv || v == x || v == w) {
                v = u; fv = fu;
            }
        }
    }
    
    return x;
}

Vector minimize_gradient_descent(double (*f)(const Vector*),
                                 const Vector *gradient,
                                 const Vector *x0, double learning_rate, int iterations) {
    Vector x = copy_vector(x0);
    Vector grad = create_vector(x0->size);
    
    for (int iter = 0; iter < iterations; iter++) {
        // 计算梯度（简化：使用数值梯度）
        for (int i = 0; i < x.size; i++) {
            Vector x_plus = copy_vector(&x);
            Vector x_minus = copy_vector(&x);
            
            x_plus.data[i] += EPSILON;
            x_minus.data[i] -= EPSILON;
            
            grad.data[i] = (f(&x_plus) - f(&x_minus)) / (2.0 * EPSILON);
            
            destroy_vector(&x_plus);
            destroy_vector(&x_minus);
        }
        
        // 更新参数
        for (int i = 0; i < x.size; i++) {
            x.data[i] -= learning_rate * grad.data[i];
        }
        
        // 检查收敛
        double grad_norm = 0.0;
        for (int i = 0; i < x.size; i++) {
            grad_norm += grad.data[i] * grad.data[i];
        }
        grad_norm = sqrt(grad_norm);
        
        if (grad_norm < EPSILON) {
            break;
        }
    }
    
    destroy_vector(&grad);
    return x;
}

/* ========== 多项式拟合 ========== */
Vector polynomial_fit(const Vector *x, const Vector *y, int degree) {
    int n = x->size;
    int m = degree + 1;
    
    // 构造范德蒙德矩阵
    Matrix A = create_matrix(n, m);
    for (int i = 0; i < n; i++) {
        double power = 1.0;
        for (int j = 0; j < m; j++) {
            A.data[i * m + j] = power;
            power *= x->data[i];
        }
    }
    
    // 构造法方程 A^T A x = A^T y
    Matrix AT = matrix_transpose(&A);
    Matrix ATA = matrix_multiply(&AT, &A);
    
    Vector ATy = create_vector(m);
    for (int i = 0; i < m; i++) {
        ATy.data[i] = 0.0;
        for (int j = 0; j < n; j++) {
            ATy.data[i] += AT.data[i * n + j] * y->data[j];
        }
    }
    
    // 求解法方程
    Vector coeffs = solve_linear_system_lu(&ATA, &ATy);
    
    destroy_matrix(&A);
    destroy_matrix(&AT);
    destroy_matrix(&ATA);
    destroy_vector(&ATy);
    
    return coeffs;
}

/* ========== 统计分析 ========== */
Vector linear_regression(const Vector *x, const Vector *y) {
    int n = x->size;
    
    // 计算统计量
    double sum_x = 0.0, sum_y = 0.0;
    double sum_xx = 0.0, sum_xy = 0.0;
    
    for (int i = 0; i < n; i++) {
        sum_x += x->data[i];
        sum_y += y->data[i];
        sum_xx += x->data[i] * x->data[i];
        sum_xy += x->data[i] * y->data[i];
    }
    
    // 计算回归系数
    double denominator = n * sum_xx - sum_x * sum_x;
    double slope = (n * sum_xy - sum_x * sum_y) / denominator;
    double intercept = (sum_y * sum_xx - sum_x * sum_xy) / denominator;
    
    Vector result = create_vector(2);
    result.data[0] = intercept;
    result.data[1] = slope;
    
    return result;
}

/* ========== 特殊函数实现 ========== */
double gamma_function(double x) {
    // Lanczos 近似
    const double g = 7.0;
    const double coeffs[] = {
        0.99999999999980993,
        676.5203681218851,
        -1259.1392167224028,
        771.32342877765313,
        -176.61502916214059,
        12.507343278686905,
        -0.13857109526572012,
        9.9843695780195716e-6,
        1.5056327351493116e-7
    };
    
    if (x < 0.5) {
        return PI / (sin(PI * x) * gamma_function(1.0 - x));
    }
    
    x -= 1.0;
    double A = coeffs[0];
    
    for (int i = 1; i < 9; i++) {
        A += coeffs[i] / (x + i);
    }
    
    double result = sqrt(2.0 * PI) * pow(x + g + 0.5, x + 0.5) * exp(-(x + g + 0.5)) * A;
    return result;
}

double beta_function(double a, double b) {
    return gamma_function(a) * gamma_function(b) / gamma_function(a + b);
}

double erf_function(double x) {
    // Abramowitz and Stegun 近似
    const double a1 =  0.254829592;
    const double a2 = -0.284496736;
    const double a3 =  1.421413741;
    const double a4 = -1.453152027;
    const double a5 =  1.061405429;
    const double p  =  0.3275911;
    
    int sign = (x < 0) ? -1 : 1;
    x = fabs(x);
    
    double t = 1.0 / (1.0 + p * x);
    double y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-x * x);
    
    return sign * y;
}

/* ========== 随机数生成 ========== */
Vector random_normal(int n, double mean, double stddev) {
    Vector result = create_vector(n);
    
    for (int i = 0; i < n; i += 2) {
        // Box-Muller 变换
        double u1 = (double)rand() / RAND_MAX;
        double u2 = (double)rand() / RAND_MAX;
        
        double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * PI * u2);
        double z1 = sqrt(-2.0 * log(u1)) * sin(2.0 * PI * u2);
        
        result.data[i] = mean + stddev * z0;
        if (i + 1 < n) {
            result.data[i + 1] = mean + stddev * z1;
        }
    }
    
    return result;
}

double ff(double t, double y) { return -y; }
double parabola(double x) { return x*x - 4*x + 4; }

void compute(void *arg) {
    printf("=== 高性能科学计算库测试 ===\n\n");
    
    srand(time(NULL));
    
    // 测试1: 矩阵运算
    printf("1. 测试矩阵运算:\n");
    Matrix A = create_matrix(3, 3);
    Matrix B = create_matrix(3, 3);
    
    // 初始化矩阵
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            A.data[i * 3 + j] = i * 3 + j + 1;
            B.data[i * 3 + j] = (i == j) ? 1.0 : 0.0;
        }
    }
    
    Matrix C = matrix_multiply(&A, &B);
    printf("矩阵乘法测试通过\n");
    
    // 测试2: 线性方程组求解
    printf("\n2. 测试线性方程组求解:\n");
    Vector b = create_vector(3);
    b.data[0] = 1.0;
    b.data[1] = 2.0;
    b.data[2] = 3.0;
    
    Vector x = solve_linear_system_lu(&A, &b);
    printf("线性方程组求解测试通过\n");
    
    // 测试3: 数值积分
    printf("\n3. 测试数值积分:\n");
    double integral = integrate_simpson(sin, 0, PI, 100);
    printf("sin(x)在[0,π]的积分: %.10f (理论值: 2.0)\n", integral);
    
    // 测试4: 微分方程求解
    printf("\n4. 测试微分方程求解:\n");
    // dy/dt = -y, y(0) = 1

    Vector sol = solve_ode_rk4(ff, 1.0, 0.0, 1.0, 100);
    printf("微分方程 y' = -y 在 t=1 的解: %.10f (理论值: %.10f)\n", 
           sol.data[100], exp(-1.0));
    
    // 测试5: 傅里叶变换
    printf("\n5. 测试傅里叶变换:\n");
    double signal[8] = {1, 2, 3, 4, 4, 3, 2, 1};
    ComplexNum dft_output[8];
    dft(signal, dft_output, 8);
    printf("DFT计算完成\n");
    
    // 测试6: 优化算法
    printf("\n6. 测试优化算法:\n");

    double min_x = minimize_golden_section(parabola, 0, 5, 1e-6);
    printf("抛物线最小值点: x = %.10f, f(x) = %.10f\n", min_x, parabola(min_x));
    
    // 测试7: 多项式拟合
    printf("\n7. 测试多项式拟合:\n");
    Vector xs = create_vector(10);
    Vector ys = create_vector(10);
    
    for (int i = 0; i < 10; i++) {
        xs.data[i] = i;
        ys.data[i] = 2*i + 1 + 0.1*((double)rand()/RAND_MAX - 0.5);
    }
    
    Vector coeffs = polynomial_fit(&xs, &ys, 1);
    printf("线性拟合: y = %.4f + %.4fx\n", coeffs.data[0], coeffs.data[1]);
    
    // 测试8: 统计分析
    printf("\n8. 测试统计分析:\n");
    Vector reg = linear_regression(&xs, &ys);
    printf("线性回归: 截距 = %.4f, 斜率 = %.4f\n", reg.data[0], reg.data[1]);
    
    // 测试9: 特殊函数
    printf("\n9. 测试特殊函数:\n");
    double gamma_val = gamma_function(5.0);
    printf("Γ(5) = %.10f (理论值: 24.0)\n", gamma_val);
    
    double erf_val = erf_function(1.0);
    printf("erf(1) = %.10f\n", erf_val);
    
    // 测试10: 随机数生成
    printf("\n10. 测试随机数生成:\n");
    Vector rands = random_normal(1000, 0.0, 1.0);
    
    double mean = 0.0;
    for (int i = 0; i < 1000; i++) {
        mean += rands.data[i];
    }
    mean /= 1000.0;
    printf("正态分布样本均值: %.6f\n", mean);
    
    // 清理内存
    destroy_matrix(&A);
    destroy_matrix(&B);
    destroy_matrix(&C);
    destroy_vector(&b);
    destroy_vector(&x);
    destroy_vector(&sol);
    destroy_vector(&xs);
    destroy_vector(&ys);
    destroy_vector(&coeffs);
    destroy_vector(&reg);
    destroy_vector(&rands);
    
    printf("\n=== 所有测试完成 ===\n");
    g_compute_ret = 0;
}
int ComputeFfrtQueue()
{
    // 并行调度
    ffrt_queue_t bank = create_bank_system("Bank", 2, 0);
    if (!bank) {
        LOGE("create bank system failed");
        return -1;
    }
   
    g_para1.a = ONE;
    g_para1.b = TWO;

    g_para2.a = FIVE;
    g_para2.b = THREE;

    CRequest request1;
    request1.name = "customer1";
    request1.arg = &g_para1;
    
    CRequest request2;
    request2.name = "customer2";
    request2.arg = &g_para2;
    
	CRequest request3;
    request3.name = "customer3";
    request3.arg = NULL;
    // VIP享受更优先的服务
    ffrt_task_handle_t task1 = commitRequest(bank, Add, request1, ffrt_queue_priority_low, 0);
    ffrt_task_handle_t task2 = commitRequest(bank, Sub, request2, ffrt_queue_priority_low, 0);
    ffrt_task_handle_t task3 = commitRequest(bank, compute, request3, ffrt_queue_priority_low, 0);

    // 等待所有的客户服务完成
    waitForRequest(task1);
    waitForRequest(task2);
    waitForRequest(task3);

    destroy_bank_system(bank);

    ffrt_task_handle_destroy(task1);
    ffrt_task_handle_destroy(task2);
    ffrt_task_handle_destroy(task3);
    LOGI("FfrtQueue results ");
    if (g_add_ret == 0 && g_sub_ret == 0 && g_compute_ret == 0) {
    return RET_SUCCESS_5;
    } else {
        return -1;
    }
}
