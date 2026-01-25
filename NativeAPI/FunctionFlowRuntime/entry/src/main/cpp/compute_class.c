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

/* ========== 常量定义 ========== */
#define ONE                    1
#define TWO                    2
#define THREE                  3
#define FIVE                   5
#define RET_SUCCESS_5          5

#define MAX_MATRIX_SIZE        1000
#define MAX_VECTOR_SIZE        10000
#define MAX_ITERATIONS         10000
#define EPSILON                (1e-12)
#define PI                     (3.14159265358979323846)
#define GOLDEN_RATIO           (1.618033988749895)
#define MAX_OPT_VARS           100
#define MAX_POLY_DEGREE        50
#define MAX_INTEGRATION_POINTS 1000
#define TEST_MATRIX_SIZE       3
#define TEST_VECTOR_SIZE       3
#define TEST_FFT_SIZE          8
#define TEST_INTEGRAL_STEPS    100
#define TEST_ODE_STEPS         100
#define TEST_OPT_TOL           (1e-6)
#define TEST_POLY_FIT_POINTS   10
#define TEST_RANDOM_SAMPLES    1000
#define TEST_POLY_DEGREE_1     1
#define TEST_NOISE_AMPLITUDE   (0.1)
#define TEST_OPT_LOWER_BOUND   0
#define TEST_OPT_UPPER_BOUND   5
#define TEST_ODE_Y0            (1.0)
#define TEST_ODE_T0            (0.0)
#define TEST_ODE_TF            (1.0)
#define TEST_GAMMA_ARG         (5.0)
#define TEST_ERF_ARG          (1.0)
#define GAMMA_G                (7.0)
#define GAMMA_COEFF_COUNT      9
#define ERF_COEFF_COUNT        5
#define DFT_SIN_COEFF          (-1.0)
#define DFT_COS_COEFF          (1.0)
#define SIMPSON_EVEN_COEFF     (2.0)
#define SIMPSON_ODD_COEFF      (4.0)
#define SIMPSON_DIVISOR        (3.0)
#define RK4_DIVISOR            (6.0)
#define RK4_HALF               (0.5)
#define RK4_K2_COEFF           (2.0)
#define RK4_K3_COEFF           (2.0)
#define POWER_ITER_INIT_VAL    (0.0)
#define RANDOM_MAX_RANGE       (1.0)
#define RANDOM_MIN_RANGE       (0.0)
#define INIT_ZERO              0
#define INIT_ONE               1
#define INIT_TWO               2
#define INIT_THREE             3
#define INIT_VALUE_1           (1.0)
#define INIT_VALUE_2           (2.0)
#define INIT_VALUE_3           (3.0)
#define INIT_VALUE_4           (4.0)
#define MATRIX_DET_SIGN_1      1
#define MATRIX_DET_SIGN_NEG    (-1)
#define MATRIX_INDEX_OFFSET    3
#define FFT_SEPARATION_FACTOR  2
#define ROMBERG_BASE           (4.0)
#define ROMBERG_POWER_BASE     (2.0)
#define ROMBERG_CONV_FACTOR    (0.5)
#define BRENT_P_COEFF_1        (0.5)
#define BRENT_P_COEFF_2        (2.0)
#define REGRESSION_DENOM_COEFF (1.0)
#define GAMMA_APPROX_OFFSET    (0.5)
#define ERF_SIGN_POS           1
#define ERF_SIGN_NEG           (-1)
#define ERF_T_DIVISOR          (1.0)
#define ERF_EXP_COEFF          (-1.0)
#define BOX_MULLER_COEFF       (-2.0)
#define NORM_MEAN              (0.0)
#define NORM_STDDEV            (1.0)
#define RET_SUCCESS            0
#define RET_FAILURE            (-1)
#define FFRT_QUEUE_PRIORITY    ffrt_queue_priority_low
#define FFRT_QUEUE_FLAG        0
#define DEFAULT_STRIDE         1
#define DEFAULT_INCREMENT      1
#define DEFAULT_DECREMENT      1
#define DEFAULT_MULTIPLIER     (2.0)
#define DEFAULT_DIVISOR        (2.0)

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
    double coeffs[MAX_POLY_DEGREE + INIT_ONE];
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
int g_addRet;
int g_subRet;
int g_computeRet;

/* ========== 函数声明 ========== */
// 内存管理
Matrix CreateMatrix(int rows, int cols);
void DestroyMatrix(Matrix *mat);
Vector CreateVector(int size);
void DestroyVector(Vector *vec);
Matrix CopyMatrix(const Matrix *src);
Vector CopyVector(const Vector *src);

// 基本运算
Matrix MatrixAdd(const Matrix *matrixA, const Matrix *matrixB);
Matrix MatrixSubtract(const Matrix *matrixA, const Matrix *matrixB);
Matrix MatrixMultiply(const Matrix *matrixA, const Matrix *matrixB);
Matrix MatrixScalarMultiply(const Matrix *matrix, double scalar);
Vector VectorAdd(const Vector *vectorA, const Vector *vectorB);
Vector VectorSubtract(const Vector *vectorA, const Vector *vectorB);
double VectorDot(const Vector *vectorA, const Vector *vectorB);
Matrix MatrixTranspose(const Matrix *matrix);
Matrix MatrixInverse(const Matrix *matrix);
double MatrixDeterminant(const Matrix *matrix);

// 线性方程组求解
Vector SolveLinearSystem(const Matrix *matrix, const Vector *b);
Vector SolveLinearSystemLu(const Matrix *matrix, const Vector *b);
Matrix LuDecomposition(const Matrix *matrix);
Vector ForwardSubstitution(const Matrix *lowerMatrix, const Vector *b);
Vector BackwardSubstitution(const Matrix *upperMatrix, const Vector *b);

// 特征值/特征向量
Matrix QrDecomposition(const Matrix *matrix);
void QrAlgorithm(const Matrix *matrix, double *eigenvalues, Matrix *eigenvectors);
double PowerIteration(const Matrix *matrix, Vector *eigenvector);
double InversePowerIteration(const Matrix *matrix, Vector *eigenvector, double sigma);

// 数值积分
double IntegrateTrapezoidal(double (*f)(double), double a, double b, int n);
double IntegrateSimpson(double (*f)(double), double a, double b, int n);
double IntegrateRomberg(double (*f)(double), double a, double b, double tol);
double IntegrateGaussian(double (*f)(double), double a, double b, int n);
double IntegrateMonteCarlo(double (*f)(double), double a, double b, int samples);

// 微分方程求解
Vector SolveOdeEuler(double (*f)(double, double), double y0,
    double t0, double tf, int steps);
Vector SolveOdeRk4(double (*f)(double, double), double y0,
    double t0, double tf, int steps);
Vector SolveOdeAdaptive(double (*f)(double, double), double y0,
    double t0, double tf, double tol);
Vector SolveOdeSystem(void (*f)(double, const Vector*, Vector*),
    const Vector *y0, double t0, double tf, int steps);

// 傅里叶变换
void Dft(const double *input, ComplexNum *output, int n);
void Idft(const ComplexNum *input, double *output, int n);
void Fft(ComplexNum *data, int n);
void Ifft(ComplexNum *data, int n);
void FftReal(const double *input, ComplexNum *output, int n);

// 优化算法
double MinimizeGoldenSection(double (*f)(double), double a, double b, double tol);
double MinimizeBrent(double (*f)(double), double a, double b, double c, double tol);
Vector MinimizeGradientDescent(double (*f)(const Vector*),
    const Vector *gradient, const Vector *x0, double learningRate, int iterations);
Vector MinimizeConjugateGradient(double (*f)(const Vector*),
    const Vector *gradient, const Matrix *matrix, const Vector *b,
    const Vector *x0, int iterations);

// 插值与逼近
Vector PolynomialFit(const Vector *x, const Vector *y, int degree);
double PolynomialEval(const Polynomial *p, double x);
Vector SplineInterpolation(const Vector *x, const Vector *y);
double SplineEval(const Vector *x, const Vector *y, const Vector *coeffs, double xi);

// 统计分析
Vector LinearRegression(Vector *x, Vector *y);
double CorrelationCoefficient(const Vector *x, const Vector *y);
Vector MovingAverage(const Vector *data, int window);
void ComputeStatistics(const Vector *data, double *mean, double *variance,
    double *skewness, double *kurtosis);

// 特殊函数
double GammaFunction(double x);
double BetaFunction(double a, double b);
double BesselJ0(double x);
double BesselJ1(double x);
double ErfFunction(double x);
double LegendrePoly(int n, double x);
double ChebyshevPoly(int n, double x);

// 随机数生成
Vector RandomUniform(int n, double a, double b);
Vector RandomNormal(int n, double mean, double stddev);
Vector RandomExponential(int n, double lambda);
Matrix RandomMatrix(int rows, int cols, double min_val, double maxVal);

// 工具函数
void PrintMatrix(const Matrix *matrix, const char *name);
void PrintVector(const Vector *vector, const char *name);
double MatrixNorm(const Matrix *matrix, int p);
double VectorNorm(const Vector *vector, int p);
bool IsSymmetric(const Matrix *matrix);
bool IsPositiveDefinite(const Matrix *matrix);

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
    g_addRet = RET_SUCCESS;
}

void Sub(void *arg)
{
    struct ParaStruct *para2 = (struct ParaStruct *)arg;
    int a = para2->a;
    int b = para2->b;
    g_subRet = RET_SUCCESS;
}

/* ========== 内存管理实现 ========== */
Matrix CreateMatrix(int rows, int cols)
{
    Matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    mat.data = (double*)malloc(rows * cols * sizeof(double));
    if (mat.data == NULL) {
        fprintf(stderr, "内存分配失败\n");
        exit(EXIT_FAILURE);
    }
    memset(mat.data, INIT_ZERO, rows * cols * sizeof(double));
    return mat;
}

void DestroyMatrix(Matrix *mat)
{
    if (mat->data != NULL) {
        free(mat->data);
        mat->data = NULL;
    }
    mat->rows = INIT_ZERO;
    mat->cols = INIT_ZERO;
}

Vector CreateVector(int size)
{
    Vector vec;
    vec.size = size;
    if (size <= 0) {
        fprintf(stderr, "无效的内存申请大小\n");
        exit(EXIT_FAILURE);
    }
    vec.data = (double*)malloc(size * sizeof(double));
    if (vec.data == NULL) {
        fprintf(stderr, "内存分配失败\n");
        exit(EXIT_FAILURE);
    }
    memset(vec.data, INIT_ZERO, size * sizeof(double));
    return vec;
}

void DestroyVector(Vector *vec)
{
    if (vec->data != NULL) {
        free(vec->data);
        vec->data = NULL;
    }
    vec->size = INIT_ZERO;
}

Matrix CopyMatrix(const Matrix *src)
{
    Matrix dst = CreateMatrix(src->rows, src->cols);
    memcpy(dst.data, src->data, src->rows * src->cols * sizeof(double));
    return dst;
}

Vector CopyVector(const Vector *src)
{
    Vector dst = CreateVector(src->size);
    memcpy(dst.data, src->data, src->size * sizeof(double));
    return dst;
}

/* ========== 基本运算实现 ========== */
Matrix MatrixAdd(const Matrix *matrixA, const Matrix *matrixB)
{
    if (matrixA->rows != matrixB->rows || matrixA->cols != matrixB->cols) {
        fprintf(stderr, "矩阵维度不匹配\n");
        exit(EXIT_FAILURE);
    }

    Matrix result = CreateMatrix(matrixA->rows, matrixA->cols);
    int total = matrixA->rows * matrixA->cols;

    for (int i = INIT_ZERO; i < total; i++) {
        result.data[i] = matrixA->data[i] + matrixB->data[i];
    }

    return result;
}

Matrix MatrixMultiply(const Matrix *matrixA, const Matrix *matrixB)
{
    if (matrixA->cols != matrixB->rows) {
        fprintf(stderr, "矩阵维度不匹配，无法相乘\n");
        exit(EXIT_FAILURE);
    }

    Matrix result = CreateMatrix(matrixA->rows, matrixB->cols);

    for (int i = INIT_ZERO; i < matrixA->rows; i++) {
        for (int j = INIT_ZERO; j < matrixB->cols; j++) {
            double sum = INIT_VALUE_1;
            for (int k = INIT_ZERO; k < matrixA->cols; k++) {
                sum += matrixA->data[i * matrixA->cols + k] * matrixB->data[k * matrixB->cols + j];
            }
            result.data[i * matrixB->cols + j] = sum;
        }
    }

    return result;
}

Matrix MatrixTranspose(const Matrix *matrix)
{
    Matrix result = CreateMatrix(matrix->cols, matrix->rows);

    for (int i = INIT_ZERO; i < matrix->rows; i++) {
        for (int j = INIT_ZERO; j < matrix->cols; j++) {
            result.data[j * matrix->rows + i] = matrix->data[i * matrix->cols + j];
        }
    }

    return result;
}

double MatrixDeterminant(const Matrix *matrix)
{
    if (matrix->rows != matrix->cols) {
        fprintf(stderr, "只有方阵才有行列式\n");
        exit(EXIT_FAILURE);
    } else {
        int n = matrix->rows;

        // 1x1 矩阵
        if (n == INIT_ONE) {
            return matrix->data[INIT_ZERO];
        }

        // 2x2 矩阵
        if (n == INIT_TWO) {
            return matrix->data[INIT_ZERO] * matrix->data[MATRIX_INDEX_OFFSET] -
                matrix->data[INIT_ONE] * matrix->data[INIT_TWO];
        }

        // 递归计算行列式
        double det = INIT_VALUE_1;
        int sign = MATRIX_DET_SIGN_1;

        for (int j = INIT_ZERO; j < n; j++) {
            // 创建子矩阵
            Matrix submat = CreateMatrix(n - INIT_ONE, n - INIT_ONE);
            int subI = INIT_ZERO;

            for (int i = INIT_ONE; i < n; i++) {
                int subJ = INIT_ZERO;
                for (int k = INIT_ZERO; k < n; k++) {
                    if (k == j) {
                        continue;
                    }
                    submat.data[subI * (n - INIT_ONE) + subJ] = matrix->data[i * n + k];
                    subJ++;
                }
                subI++;
            }

            det += sign * matrix->data[j] * MatrixDeterminant(&submat);
            sign = MATRIX_DET_SIGN_NEG;

            DestroyMatrix(&submat);
        } // 继续计算行列式的代码
        return det;
    }
}

/* ========== LU分解实现 ========== */
Matrix LuDecomposition(const Matrix *matrix)
{
    if (matrix->rows != matrix->cols) {
        fprintf(stderr, "LU分解需要方阵\n");
        exit(EXIT_FAILURE);
    }

    int n = matrix->rows;
    Matrix lu = CopyMatrix(matrix);

    for (int k = INIT_ZERO; k < n - INIT_ONE; k++) {
        if (fabs(lu.data[k * n + k]) < EPSILON) {
            fprintf(stderr, "主元为零，无法进行LU分解\n");
            exit(EXIT_FAILURE);
        }

        for (int i = k + INIT_ONE; i < n; i++) {
            lu.data[i * n + k] /= lu.data[k * n + k];

            for (int j = k + INIT_ONE; j < n; j++) {
                lu.data[i * n + j] -= lu.data[i * n + k] * lu.data[k * n + j];
            }
        }
    }

    return lu;
}

Vector ForwardSubstitution(const Matrix *lowerMatrix, const Vector *b)
{
    int n = lowerMatrix->rows;
    Vector x = CreateVector(n);

    for (int i = INIT_ZERO; i < n; i++) {
        double sum = INIT_VALUE_1;
        for (int j = INIT_ZERO; j < i; j++) {
            sum += lowerMatrix->data[i * n + j] * x.data[j];
        }
        x.data[i] = (b->data[i] - sum) / lowerMatrix->data[i * n + i];
    }

    return x;
}

Vector BackwardSubstitution(const Matrix *upperMatrix, const Vector *b)
{
    int n = upperMatrix->rows;
    Vector x = CreateVector(n);

    for (int i = n - INIT_ONE; i >= INIT_ZERO; i--) {
        double sum = INIT_VALUE_1;
        for (int j = i + INIT_ONE; j < n; j++) {
            sum += upperMatrix->data[i * n + j] * x.data[j];
        }
        x.data[i] = (b->data[i] - sum) / upperMatrix->data[i * n + i];
    }

    return x;
}

/* ========== 线性方程组求解 ========== */
Vector SolveLinearSystemLu(const Matrix *matrix, const Vector *b)
{
    Matrix lu = LuDecomposition(matrix);
    int n = matrix->rows;

    // 从LU分解中提取L和U
    Matrix L = CreateMatrix(n, n);
    Matrix U = CreateMatrix(n, n);

    for (int i = INIT_ZERO; i < n; i++) {
        for (int j = INIT_ZERO; j < n; j++) {
            if (i > j) {
                L.data[i * n + j] = lu.data[i * n + j];
                U.data[i * n + j] = INIT_VALUE_1;
            } else if (i == j) {
                L.data[i * n + j] = INIT_VALUE_1;
                U.data[i * n + j] = lu.data[i * n + j];
            } else {
                L.data[i * n + j] = INIT_VALUE_1;
                U.data[i * n + j] = lu.data[i * n + j];
            }
        }
    }

    // 解 Ly = b
    Vector y = ForwardSubstitution(&L, b);

    // 解 Ux = y
    Vector x = BackwardSubstitution(&U, &y);

    DestroyMatrix(&lu);
    DestroyMatrix(&L);
    DestroyMatrix(&U);
    DestroyVector(&y);

    return x;
}

/* ========== QR分解实现 ========== */
Matrix QrDecomposition(const Matrix *matrix)
{
    int m = matrix->rows;
    int n = matrix->cols;
    Matrix q = CreateMatrix(m, n);
    Matrix r = CreateMatrix(n, n);
    Matrix v = CopyMatrix(matrix);

    for (int j = INIT_ZERO; j < n; j++) {
        // 计算第j列的范数
        double norm = INIT_VALUE_1;
        for (int i = INIT_ZERO; i < m; i++) {
            norm += v.data[i * n + j] * v.data[i * n + j];
        }
        norm = sqrt(norm);

        // 构造Householder向量
        r.data[j * n + j] = norm;

        if (norm > EPSILON) {
            for (int i = INIT_ZERO; i < m; i++) {
                q.data[i * n + j] = norm != 0 ? v.data[i * n + j] / norm : 0;
                q.data[i * n + j] = v.data[i * n + j] / norm;
            }

            // 更新剩余列
            for (int k = j + INIT_ONE; k < n; k++) {
                double dot = INIT_VALUE_1;
                for (int i = INIT_ZERO; i < m; i++) {
                    dot += q.data[i * n + j] * v.data[i * n + k];
                }
                r.data[j * n + k] = dot;

                for (int i = INIT_ZERO; i < m; i++) {
                    v.data[i * n + k] -= DEFAULT_MULTIPLIER * q.data[i * n + j] * dot;
                }
            }
        }
    }

    DestroyMatrix(&v);
    return q;  // 返回Q矩阵，R矩阵在过程中计算
}

/* ========== 特征值计算 ========== */
double PowerIteration(const Matrix *matrix, Vector *eigenvector)
{
    int n = matrix->rows;
    Vector b = CreateVector(n);

    // 初始化随机向量
    for (int i = INIT_ZERO; i < n; i++) {
        b.data[i] = (double)rand() / RAND_MAX;
    }

    // 归一化
    double norm = POWER_ITER_INIT_VAL;
    for (int i = INIT_ZERO; i < n; i++) {
        norm += b.data[i] * b.data[i];
    }
    norm = sqrt(norm);
    for (int i = INIT_ZERO; i < n; i++) {
        if (norm != 0) {
            b.data[i] /= norm;;
        } else {
            b.data[i] = 0;
        }
    }

    // 幂迭代
    double eigenvalue = POWER_ITER_INIT_VAL;
    for (int iter = INIT_ZERO; iter < MAX_ITERATIONS; iter++) {
        // 计算 ab
        Vector ab = CreateVector(n);
        for (int i = INIT_ZERO; i < n; i++) {
            ab.data[i] = POWER_ITER_INIT_VAL;
            for (int j = INIT_ZERO; j < n; j++) {
                ab.data[i] += matrix->data[i * n + j] * b.data[j];
            }
        }

        // 计算 Rayleigh 商
        double numerator = POWER_ITER_INIT_VAL;
        double denominator = POWER_ITER_INIT_VAL;
        for (int i = INIT_ZERO; i < n; i++) {
            numerator += b.data[i] * ab.data[i];
            denominator += b.data[i] * b.data[i];
        }
        double newEigenvalue = numerator / denominator;

        // 检查收敛
        if (fabs(newEigenvalue - eigenvalue) < EPSILON) {
            eigenvalue = newEigenvalue;
            // 复制特征向量
            for (int i = INIT_ZERO; i < n; i++) {
                eigenvector->data[i] = b.data[i];
            }
            DestroyVector(&ab);
            break;
        }

        eigenvalue = newEigenvalue;

        // 归一化新向量
        norm = POWER_ITER_INIT_VAL;
        for (int i = INIT_ZERO; i < n; i++) {
            norm += ab.data[i] * ab.data[i];
        }
        norm = sqrt(norm);
        for (int i = INIT_ZERO; i < n; i++) {
            b.data[i] = ab.data[i] / norm;
        }

        DestroyVector(&ab);
    }

    DestroyVector(&b);
    return eigenvalue;
}

/* ========== 数值积分实现 ========== */
double IntegrateTrapezoidal(double (*f)(double), double a, double b, int n)
{
    if (n == 0) {
        return 0;
    }
    double h = (b - a) / n;
    double sum = DEFAULT_DIVISOR * (f(a) + f(b));

    for (int i = INIT_ONE; i < n; i++) {
        double x = a + i * h;
        sum += f(x);
    }

    return sum * h;
}

double IntegrateSimpson(double (*f)(double), double a, double b, int n)
{
    if (n % INIT_TWO != INIT_ZERO) {
        n++;
    }
    // 确保n为偶数
    if (n == 0) {
        return 0;
        // 避免除零错误
    }
    double h = (b - a) / n;
    double sum = f(a) + f(b);

    for (int i = INIT_ONE; i < n; i++) {
        double x = a + i * h;
        if (i % INIT_TWO == INIT_ZERO) {
            sum += SIMPSON_EVEN_COEFF * f(x);
        } else {
            sum += SIMPSON_ODD_COEFF * f(x);
        }
    }

    return sum * h / SIMPSON_DIVISOR;
}

double IntegrateRomberg(double (*f)(double), double a, double b, double tol)
{
    double r[MAX_ITERATIONS][MAX_ITERATIONS];
    int k = INIT_ZERO;

    // 初始梯形公式
    r[INIT_ZERO][INIT_ZERO] = (b - a) * (f(a) + f(b)) / DEFAULT_MULTIPLIER;

    for (k = INIT_ONE; k < MAX_ITERATIONS; k++) {
        // 计算梯形公式的递归
        int n = INIT_ONE << k;
        // 2^k
        double h = (b - a) / n;
        double sum = INIT_VALUE_1;

        for (int i = INIT_ONE; i < n; i += INIT_TWO) {
            double x = a + i * h;
            sum += f(x);
        }

        r[k][INIT_ZERO] = ROMBERG_CONV_FACTOR * r[k - INIT_ONE][INIT_ZERO] + h * sum;

        // Richardson 外推
        for (int j = INIT_ONE; j <= k; j++) {
            r[k][j] = r[k][j - INIT_ONE] +
                (r[k][j - INIT_ONE] - r[k - INIT_ONE][j - INIT_ONE]) /
                (pow(ROMBERG_BASE, j) - INIT_VALUE_1);
        }

        // 检查收敛
        if (k > INIT_ZERO && fabs(r[k][k] - r[k - INIT_ONE][k - INIT_ONE]) < tol) {
            return r[k][k];
        }
    }

    return r[k - INIT_ONE][k - INIT_ONE];
}

/* ========== 微分方程求解 ========== */
Vector SolveOdeRk4(double (*f)(double, double), double y0,
    double t0, double tf, int steps)
{
    double h = (tf - t0) / (steps != INIT_ZERO ? steps : INIT_ONE);
    Vector solution = CreateVector(steps + INIT_ONE);

    solution.data[INIT_ZERO] = y0;
    double t = t0;
    double y = y0;

    for (int i = INIT_ZERO; i < steps; i++) {
        double k1 = h * f(t, y);
        double k2 = h * f(t + h / DEFAULT_MULTIPLIER, y + k1 / DEFAULT_MULTIPLIER);
        double k3 = h * f(t + h / DEFAULT_MULTIPLIER, y + k2 / DEFAULT_MULTIPLIER);
        double k4 = h * f(t + h, y + k3);

        y = y + (k1 + RK4_K2_COEFF * k2 + RK4_K3_COEFF * k3 + k4) / RK4_DIVISOR;
        t = t + h;

        solution.data[i + INIT_ONE] = y;
    }

    return solution;
}

/* ========== 傅里叶变换实现 ========== */
void Dft(const double *input, ComplexNum *output, int n)
{
    for (int k = INIT_ZERO; k < n; k++) {
        double real = INIT_VALUE_1;
        double imag = INIT_VALUE_1;

        for (int t = INIT_ZERO; t < n; t++) {
            double angle = (n != 0) ? DEFAULT_MULTIPLIER * PI * k * t / n : 0;
            real += input[t] * cos(angle);
            imag += DFT_SIN_COEFF * input[t] * sin(angle);
        }

        output[k].real = real;
        output[k].imag = imag;
    }
}

void Idft(const ComplexNum *input, double *output, int n)
{
    for (int k = INIT_ZERO; k < n; k++) {
        double real = INIT_VALUE_1;
        double imag = INIT_VALUE_1;

        for (int t = INIT_ZERO; t < n; t++) {
            if (n != 0) {
                double angle = DEFAULT_MULTIPLIER * PI * k * t / n;
                real += input[t].real * cos(angle) - input[t].imag * sin(angle);
                imag += input[t].real * sin(angle) + input[t].imag * cos(angle);
            }
        }
        if (n != 0) {
            output[k] = real / n;
        }

    }
}

void Fft(ComplexNum *data, int n)
{
    if (n <= INIT_ONE) {
        return;
    }

    // 分离偶数和奇数项
    ComplexNum *even = (ComplexNum*)malloc(n / FFT_SEPARATION_FACTOR * sizeof(ComplexNum));
    ComplexNum *odd = (ComplexNum*)malloc(n / FFT_SEPARATION_FACTOR * sizeof(ComplexNum));

    for (int i = INIT_ZERO; i < n / FFT_SEPARATION_FACTOR; i++) {
        even[i] = data[i * FFT_SEPARATION_FACTOR];
        odd[i] = data[i * FFT_SEPARATION_FACTOR + INIT_ONE];
    }

    // 递归计算
    Fft(even, n / FFT_SEPARATION_FACTOR);
    Fft(odd, n / FFT_SEPARATION_FACTOR);

    // 合并结果
    for (int k = INIT_ZERO; k < n / FFT_SEPARATION_FACTOR; k++) {
        if (n != 0) { // 检查 n 是否为 0
            double angle = DEFAULT_MULTIPLIER * DFT_SIN_COEFF * PI * k / n;
            ComplexNum t;
            t.real = cos(angle) * odd[k].real - sin(angle) * odd[k].imag;
            t.imag = sin(angle) * odd[k].real + cos(angle) * odd[k].imag;

            data[k].real = even[k].real + t.real;
            data[k].imag = even[k].imag + t.imag;
            data[k + n / FFT_SEPARATION_FACTOR].real = even[k].real - t.real;
            data[k + n / FFT_SEPARATION_FACTOR].imag = even[k].imag - t.imag;
        }
    }

    free(even);
    free(odd);
}

/* ========== 优化算法实现 ========== */
double MinimizeGoldenSection(double (*f)(double), double a, double b, double tol)
{
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

    return (a + b) / DEFAULT_MULTIPLIER;
}

double MinimizeBrent(double (*f)(double), double a, double b, double c, double tol)
{
    double x = b;
    double fx = f(x);
    double w = b;
    double fw = fx;
    double v = b;
    double fv = fx;

    double d = INIT_VALUE_1;
    double e = INIT_VALUE_1;

    for (int iter = INIT_ZERO; iter < MAX_ITERATIONS; iter++) {
        double xm = (a + b) / DEFAULT_MULTIPLIER;
        double tol1 = tol * fabs(x) + EPSILON;
        double tol2 = DEFAULT_MULTIPLIER * tol1;

        // 检查收敛
        if (fabs(x - xm) <= (tol2 - BRENT_P_COEFF_1 * (b - a))) {
            return x;
        }

        // 尝试抛物线插值
        if (fabs(e) > tol1) {
            double r = (x - w) * (fx - fv);
            double q = (x - v) * (fx - fw);
            double p = (x - v) * q - (x - w) * r;
            q = BRENT_P_COEFF_2 * (q - r);

            if (q > INIT_VALUE_1) {
                p = -p;
            }
            q = fabs(q);

            double etemp = e;
            e = d;

            // 检查抛物线步长是否可接受
            if (fabs(p) < fabs(BRENT_P_COEFF_1 * q * etemp) && p > q * (a - x) && p < q * (b - x)) {
                if (q != 0) {
                    d = p / q;
                    double u = x + d;
                    if (u - a < tol2 || b - u < tol2) {
                        d = (x < xm) ? tol1 : -tol1;
                    }
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
        double u = x + ((fabs(d) >= tol1) ? d : ((d > INIT_VALUE_1) ? tol1 : -tol1));
        double fu = f(u);

        // 更新区间
        if (fu <= fx) {
            if (u >= x) {
                a = x;
            } else {
                b = x;
            }
            v = w;
            fv = fw;
            w = x;
            fw = fx;
            x = u;
            fx = fu;
        } else {
            if (u < x) {
                a = u;
            } else {
                b = u;
            }
            if (fu <= fw || w == x) {
                v = w;
                fv = fw;
                w = u;
                fw = fu;
            } else if (fu <= fv || v == x || v == w) {
                v = u;
                fv = fu;
            }
        }
    }

    return x;
}

Vector MinimizeGradientDescent(double (*f)(const Vector*), const Vector *gradient, const Vector *x0, double learningRate, int iterations)
{
    Vector x = CopyVector(x0);
    Vector grad = CreateVector(x0->size);

    for (int iter = INIT_ZERO; iter < iterations; iter++) {
        // 计算梯度（简化：使用数值梯度）
        for (int i = INIT_ZERO; i < x.size; i++) {
            Vector xPlus = CopyVector(&x);
            Vector xMinus = CopyVector(&x);

            xPlus.data[i] += EPSILON;
            xMinus.data[i] -= EPSILON;

            grad.data[i] = (f(&xPlus) - f(&xMinus)) / (DEFAULT_MULTIPLIER * EPSILON);

            DestroyVector(&xPlus);
            DestroyVector(&xMinus);
        }

        // 更新参数
        for (int i = INIT_ZERO; i < x.size; i++) {
            x.data[i] -= learningRate * grad.data[i];
        }

        // 检查收敛
        double gradNorm = INIT_VALUE_1;
        for (int i = INIT_ZERO; i < x.size; i++) {
            gradNorm += grad.data[i] * grad.data[i];
        }
        gradNorm = sqrt(gradNorm);

        if (gradNorm < EPSILON) {
            break;
        }
    }

    DestroyVector(&grad);
    return x;
}

/* ========== 多项式拟合 ========== */
Vector PolynomialFit(const Vector *x, const Vector *y, int degree)
{
    int n = x->size;
    int m = degree + INIT_ONE;

    // 构造范德蒙德矩阵
    Matrix a = CreateMatrix(n, m);
    for (int i = INIT_ZERO; i < n; i++) {
        double power = INIT_VALUE_1;
        for (int j = INIT_ZERO; j < m; j++) {
            a.data[i * m + j] = power;
            power *= x->data[i];
        }
    }

    // 构造法方程 A^T A x = A^T y
    Matrix at = MatrixTranspose(&a);
    Matrix ata = MatrixMultiply(&at, &a);

    Vector aty = CreateVector(m);
    for (int i = INIT_ZERO; i < m; i++) {
        aty.data[i] = INIT_VALUE_1;
        for (int j = INIT_ZERO; j < n; j++) {
            aty.data[i] += at.data[i * n + j] * y->data[j];
        }
    }

    // 求解法方程
    Vector coeffs = SolveLinearSystemLu(&ata, &aty);

    DestroyMatrix(&a);
    DestroyMatrix(&at);
    DestroyMatrix(&ata);
    DestroyVector(&aty);

    return coeffs;
}

/* ========== 统计分析 ========== */
Vector LinearRegression(Vector *x, Vector *y)
{
    int n = x->size;
    double sumX = 0.0;
    double sumY = 0.0;
    double sumXx = 0.0;
    double sumXy = 0.0;

    for (int i = INIT_ZERO; i < n; i++) {
        sumX += x->data[i];
        sumY += y->data[i];
        sumXx += x->data[i] * x->data[i];
        sumXy += x->data[i] * y->data[i];
    }

    // 计算回归系数
    double denominator = n * sumXx - sumX * sumX;
    if (denominator == 0.0) {
        LOGE("LinearRegression: singular matrix");
        Vector result = CreateVector(INIT_ONE);
        result.data[INIT_ZERO] = 0.0;
        return result;
    }

    double slope = (n * sumXy - sumX * sumY) / denominator;
    double intercept = (sumY * sumXx - sumX * sumXy) / denominator;

    Vector result = CreateVector(INIT_TWO);
    result.data[INIT_ZERO] = intercept;
    result.data[INIT_ONE] = slope;

    return result;
}
/* ========== 特殊函数实现 ========== */
double GammaFunction(double x)
{
    // Lanczos 近似
    const double g = GAMMA_G;
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

    if (x < DEFAULT_DIVISOR) {
        return PI / (sin(PI * x) * GammaFunction(INIT_VALUE_1 - x));
    }

    x -= INIT_VALUE_1;
    double a = coeffs[INIT_ZERO];

    for (int i = INIT_ONE; i < GAMMA_COEFF_COUNT; i++) {
        a += coeffs[i] / (x + i);
    }

    double result = sqrt(DEFAULT_MULTIPLIER * PI) *
                    pow(x + g + GAMMA_APPROX_OFFSET, x + GAMMA_APPROX_OFFSET) *
                    exp(-(x + g + GAMMA_APPROX_OFFSET)) * a;
    return result;
}
double BetaFunction(double a, double b)
{
    return GammaFunction(a) * GammaFunction(b) / GammaFunction(a + b);
}

double ErfFunction(double x)
{
    // Abramowitz and Stegun 近似
    const double a1 =  0.254829592;
    const double a2 = -0.284496736;
    const double a3 =  1.421413741;
    const double a4 = -1.453152027;
    const double a5 =  1.061405429;
    const double p  =  0.3275911;

    int sign = (x < INIT_VALUE_1) ? ERF_SIGN_NEG : ERF_SIGN_POS;
    x = fabs(x);

    double t = ERF_T_DIVISOR / (ERF_T_DIVISOR + p * x);
    double y = ERF_T_DIVISOR - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(ERF_EXP_COEFF * x * x);

    return sign * y;
}

/* ========== 随机数生成 ========== */
Vector RandomNormal(int n, double mean, double stddev)
{
    Vector result = CreateVector(n);

    for (int i = INIT_ZERO; i < n; i += INIT_TWO) {
        // Box-Muller 变换
        double u1 = (double)rand() / RAND_MAX;
        double u2 = (double)rand() / RAND_MAX;

        double z0 = sqrt(BOX_MULLER_COEFF * log(u1)) * cos(DEFAULT_MULTIPLIER * PI * u2);
        double z1 = sqrt(BOX_MULLER_COEFF * log(u1)) * sin(DEFAULT_MULTIPLIER * PI * u2);

        result.data[i] = mean + stddev * z0;
        if (i + INIT_ONE < n) {
            result.data[i + INIT_ONE] = mean + stddev * z1;
        }
    }

    return result;
}

double ff(double t, double y)
{
    return -y;
}
double Parabola(double x)
{
    return x * x - INIT_VALUE_4 * x + INIT_VALUE_4;
}

void Compute(void *arg)
{
    printf("=== 高性能科学计算库测试 ===\n\n");

    srand(time(NULL));

    // 测试1: 矩阵运算
    printf("1. 测试矩阵运算:\n");
    Matrix a = CreateMatrix(TEST_MATRIX_SIZE, TEST_MATRIX_SIZE);
    Matrix b = CreateMatrix(TEST_MATRIX_SIZE, TEST_MATRIX_SIZE);

    // 初始化矩阵
    for (int i = INIT_ZERO; i < TEST_MATRIX_SIZE; i++) {
        for (int j = INIT_ZERO; j < TEST_MATRIX_SIZE; j++) {
            a.data[i * TEST_MATRIX_SIZE + j] = i * TEST_MATRIX_SIZE + j + INIT_ONE;
            b.data[i * TEST_MATRIX_SIZE + j] = (i == j) ? INIT_VALUE_1 : INIT_VALUE_1;
        }
    }

    Matrix c = MatrixMultiply(&a, &b);
    printf("矩阵乘法测试通过\n");

    // 测试2: 线性方程组求解
    printf("\n2. 测试线性方程组求解:\n");
    Vector ab = CreateVector(TEST_VECTOR_SIZE);
    ab.data[INIT_ZERO] = INIT_VALUE_1;
    ab.data[INIT_ONE] = INIT_VALUE_2;
    ab.data[INIT_TWO] = INIT_VALUE_3;

    Vector x = SolveLinearSystemLu(&a, &ab);
    printf("线性方程组求解测试通过\n");

    // 测试3: 数值积分
    printf("\n3. 测试数值积分:\n");
    double integral = IntegrateSimpson(sin, INIT_VALUE_1, PI, TEST_INTEGRAL_STEPS);
    printf("sin(x)在[0,π]的积分: %.10f (理论值: 2.0)\n", integral);

    // 测试4: 微分方程求解
    printf("\n4. 测试微分方程求解:\n");
    // dy/dt = -y, y(0) = 1

    Vector sol = SolveOdeRk4(ff, TEST_ODE_Y0, TEST_ODE_T0, TEST_ODE_TF, TEST_ODE_STEPS);
    printf("微分方程 y' = -y 在 t=1 的解: %.10f (理论值: %.10f)\n",
           sol.data[TEST_ODE_STEPS], exp(-INIT_VALUE_1));

    // 测试5: 傅里叶变换
    printf("\n5. 测试傅里叶变换:\n");
    double signal[TEST_FFT_SIZE] = {INIT_VALUE_1, INIT_VALUE_2, INIT_VALUE_3, INIT_VALUE_4,
                                   INIT_VALUE_4, INIT_VALUE_3, INIT_VALUE_2, INIT_VALUE_1};
    ComplexNum dftOutput[TEST_FFT_SIZE];
    Dft(signal, dftOutput, TEST_FFT_SIZE);
    printf("DFT计算完成\n");

    // 测试6: 优化算法
    printf("\n6. 测试优化算法:\n");

    double minX = MinimizeGoldenSection(Parabola, TEST_OPT_LOWER_BOUND, TEST_OPT_UPPER_BOUND, TEST_OPT_TOL);
    printf("抛物线最小值点: x = %.10f, f(x) = %.10f\n", minX, Parabola(minX));

    // 测试7: 多项式拟合
    printf("\n7. 测试多项式拟合:\n");
    Vector xs = CreateVector(TEST_POLY_FIT_POINTS);
    Vector ys = CreateVector(TEST_POLY_FIT_POINTS);

    for (int i = INIT_ZERO; i < TEST_POLY_FIT_POINTS; i++) {
        xs.data[i] = i;
        ys.data[i] = INIT_TWO * i + INIT_ONE + TEST_NOISE_AMPLITUDE * ((double)rand() / RAND_MAX - DEFAULT_DIVISOR);
    }

    Vector coeffs = PolynomialFit(&xs, &ys, TEST_POLY_DEGREE_1);
    printf("线性拟合: y = %.4f + %.4fx\n", coeffs.data[INIT_ZERO], coeffs.data[INIT_ONE]);

    // 测试8: 统计分析
    printf("\n8. 测试统计分析:\n");
    Vector reg = LinearRegression(&xs, &ys);
    printf("线性回归: 截距 = %.4f, 斜率 = %.4f\n", reg.data[INIT_ZERO], reg.data[INIT_ONE]);

    // 测试9: 特殊函数
    printf("\n9. 测试特殊函数:\n");
    double gamma_val = GammaFunction(TEST_GAMMA_ARG);
    printf("Γ(5) = %.10f (理论值: 24.0)\n", gamma_val);

    double erfVal = ErfFunction(TEST_ERF_ARG);
    printf("erf(1) = %.10f\n", erfVal);

    // 测试10: 随机数生成
    printf("\n10. 测试随机数生成:\n");
    Vector rands = RandomNormal(TEST_RANDOM_SAMPLES, NORM_MEAN, NORM_STDDEV);

    double mean = INIT_VALUE_1;
    for (int i = INIT_ZERO; i < TEST_RANDOM_SAMPLES; i++) {
        mean += rands.data[i];
    }
    mean /= TEST_RANDOM_SAMPLES;
    printf("正态分布样本均值: %.6f\n", mean);

    // 清理内存
    DestroyMatrix(&a);
    DestroyMatrix(&b);
    DestroyMatrix(&c);
    DestroyVector(&ab);
    DestroyVector(&x);
    DestroyVector(&sol);
    DestroyVector(&xs);
    DestroyVector(&ys);
    DestroyVector(&coeffs);
    DestroyVector(&reg);
    DestroyVector(&rands);

    printf("\n=== 所有测试完成 ===\n");
    g_computeRet = RET_SUCCESS;
}

int ComputeFfrtQueue()
{
    // 并行调度
    ffrt_queue_t bank = CreateBankSystem("Bank", INIT_TWO, TYPE_CONCURRENT);
    if (!bank) {
        LOGE("create bank system failed");
        return RET_FAILURE;
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
    ffrt_task_handle_t task1 = CommitRequest(bank, Add, request1, FFRT_QUEUE_PRIORITY, FFRT_QUEUE_FLAG);
    ffrt_task_handle_t task2 = CommitRequest(bank, Sub, request2, FFRT_QUEUE_PRIORITY, FFRT_QUEUE_FLAG);
    ffrt_task_handle_t task3 = CommitRequest(bank, Compute, request3, FFRT_QUEUE_PRIORITY, FFRT_QUEUE_FLAG);

    // 等待所有的客户服务完成
    WaitForRequest(task1);
    WaitForRequest(task2);
    WaitForRequest(task3);

    DestroyBankSystem(bank);

    ffrt_task_handle_destroy(task1);
    ffrt_task_handle_destroy(task2);
    ffrt_task_handle_destroy(task3);

    LOGI("FfrtQueue results ");
    if (g_addRet == RET_SUCCESS && g_subRet == RET_SUCCESS && g_computeRet == RET_SUCCESS) {
        return RET_SUCCESS_5;
    } else {
        return RET_FAILURE;
    }
}