#ifndef XE_EQUATION_H
#define XE_EQUATION_H
/*
* 文件名称：xe_Equation.h
* 摘    要：线性方程组及其求解
* 作    者：H.J.Xie
* 完成日期：2025年8月28日
*/
#include <cstring>
namespace xespice 
{
// 实数线性方程组类，使用列选主元法 LU 分解求解 Ax = B
struct Equation {
private:
    int N = 0;           // 方程组的规模（未知数个数）
    int* P = nullptr;    // 用于列主元法的行交换记录（N）
    double* A = nullptr; // 系数矩阵（N*N）
    double* X = nullptr; // 解向量（N）
    double* B = nullptr; // 常数向量（N）
    double* Y = nullptr; // 用于存储中间结果 Y 的临时向量（N）
    double* LU = nullptr; // 用于存储 LU 分解结果的临时矩阵（N*N）
public:
    //构造函数，初始化方程组规模为 n，矩阵 A 和 向量 B 会初始化为 0
    Equation(int n);
    // 获取方程组规模（未知数个数）
    int Size(); 
    //获取系数矩阵 A 的元素 A(i,j)
    double GetA(int i, int j);
    //获取常数向量 B 的元素 B(i)
    double GetB(int i); 
    //获取解向量 X 的元素 X(i)
    double GetX(int i);
    //设置系数矩阵 A 的元素 A(i,j) = val
    void SetA(int i, int j, double val);
    //设置常数向量 B 的元素 B(i) = val
    void SetB(int i, double val);
    //系数矩阵 A 的元素 A(i,j) 增加 val
    void AddA(int i, int j, double val);
    //常数向量 B 的元素 B(i) 增加 val
    void AddB(int i, double val);
    //对系数矩阵 A 进行列选主元法 LU 分解（pivotTol为最小主元容忍度，返回true表示分解成功）
    bool Factorize(double pivotTol = 1e-13); 
    //对分解后的矩阵进行前向和后向替换，求解线性方程组
    void Substitute();
    //保存当前的矩阵 A
    void SaveA(double* outA);
    //保存当前的向量 B
    void SaveB(double* outB); 
    //保存当前的向量 X
    void SaveX(double* outX); 
    //加载输入的矩阵 A
    void LoadA(double* inA);
    //加载输入的向量 B
    void LoadB(double* inB); 
    //析构函数，释放内存
    ~Equation(); 
};

inline Equation::Equation(int n) : N(n) {
    A = new double[n * n](); // 初始化为 0
    B = new double[n](); // 初始化为 0
    X = new double[n]();
    P = new int[n];
    Y = new double[n];
    LU = new double[n * n];
}

inline int Equation::Size() { 
    return N; 
}
inline double Equation::GetA(int i, int j) { 
    if ((i | j) < 0) return 0; // 忽略负索引
    else return A[i * N + j]; 
}
inline double Equation::GetB(int i) { 
    return (i < 0) ? 0 : B[i]; 
}
inline double Equation::GetX(int i) { 
    return (i < 0) ? 0 : X[i]; 
}
inline void Equation::SetA(int i, int j, double val) {
    if ((i | j) < 0) return;
    A[i * N + j] = val; 
}
inline void Equation::SetB(int i, double val) {
    if (i < 0) return; 
    B[i] = val; 
}
inline void Equation::AddA(int i, int j, double val) {
    if ((i | j) < 0) return;
    A[i * N + j] += val; 
}
inline void Equation::AddB(int i, double val) {
    if (i < 0) return; 
    B[i] += val; 
}

inline bool Equation::Factorize(double pivotTol) {
    // 初始化行交换记录
    for (int i = 0; i < N; i++) P[i] = i;
    // 列选主元 
    for (int j = 0; j < N; j++) {
        double pivot = 0;
        int k = j; // 记录当前列主元所在行
        for (int i = j; i < N; i++) {
            int ij = P[i] * N + j; //计算 A[P[i], j] 的线性索引
            double a = (A[ij] >= 0) ? A[ij] : -A[ij]; // 计算绝对值
            if (a > pivot) {
                pivot = a; // 选取最大列主元
                k = i; // 记录最大列主元所在行
            }
        }
        if (pivot < pivotTol) return false; // 主元绝对值小于容忍度，矩阵奇异，无法分解
        int temp = P[j]; P[j] = P[k]; P[k] = temp; // 交换行记录
    }
    // 根据行交换记录重排矩阵 A' 到 LU
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            LU[i * N + j] = A[P[i] * N + j];
    // 对重排后的 A' 进行 LU 分解，存储结果在 LU 中
    for (int j = 0; j < N; j++) {
        for (int i = j + 1; i < N; i++)
            LU[i * N + j] /= LU[j * N + j]; // 计算 L 的元素 L(i,j) = U(i,j) / U(j,j)
        for (int i = j + 1; i < N; i++)
            for (int k = j + 1; k < N; k++)
                LU[i * N + k] -= LU[i * N + j] * LU[j * N + k]; // 更新 U 的元素 U(i,k) -= L(i,j) * U(j,k)
    }
    return true; // 分解成功
}

inline void Equation::Substitute() {
    // 前向替换，求解 L * Y = B'
    for (int i = 0; i < N; i++) {
        Y[i] = B[P[i]]; // 初始化 Y(i) = B'(i)
        for (int j = 0; j < i; j++)
            Y[i] -= LU[i * N + j] * Y[j]; // 更新 Y(i) -= L(i,j) * Y(j)
    }
    // 后向替换，求解 U * X = Y
    for (int i = N - 1; i >= 0; i--) {
        X[i] = Y[i]; // 初始化 X(i) = Y(i)
        for (int j = N - 1; j > i; j--)
            X[i] -= LU[i * N + j] * X[j]; // 更新 X(i) -= U(i,j) * X(j)
        X[i] /= LU[i * N + i]; // 计算 X(i) /= U(i,i)
    }
}

inline void Equation::SaveA(double* outA) {
    std::memcpy(outA, A, sizeof(double) * N * N);
}
inline void Equation::SaveB(double* outB) {
    std::memcpy(outB, B, sizeof(double) * N);
}
inline void Equation::SaveX(double* outX) {
    std::memcpy(outX, X, sizeof(double) * N);
}
inline void Equation::LoadA(double* inA) {
    std::memcpy(A, inA, sizeof(double) * N * N);
}
inline void Equation::LoadB(double* inB) {
    std::memcpy(B, inB, sizeof(double) * N);
}

Equation::~Equation() {
    delete[] P;
    delete[] A;
    delete[] X;
    delete[] B;
    delete[] LU;
    delete[] Y;
}

} // namespace xespice
#endif // !XE_EQUATION_H