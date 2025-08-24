#ifndef XE_LINEAREQUATION_H
#define XE_LINEAREQUATION_H
/*
* 文件名称：xe_LinearEquation.h
* 摘    要：线性方程组及其求解
* 作    者：H.J.Xie
* 完成日期：2025年8月23日
*/
namespace xespice 
{

struct LinearEquation {
private:
    int N = 0;           // 方程组的规模（未知数个数）
    int* P = nullptr;    // 用于列主元法的行交换记录（N）
    double* A = nullptr; // 系数矩阵（N*N）
    double* X = nullptr; // 解向量（N）
    double* B = nullptr; // 常数向量（N）
    double* Y = nullptr; // 用于存储中间结果 Y 的临时向量（N）
    double* LU = nullptr; // 用于存储 LU 分解结果的临时矩阵（N*N）
public:
    //构造函数，初始化方程组规模为 n
    LinearEquation(int n); 
    //获取系数矩阵 A 的元素 A(i,j)
    double GetA(int i, int j) const; 
    //获取解向量 X 的元素 X(i)
    double GetX(int i) const; 
    //获取常数向量 B 的元素 B(i)
    double GetB(int i) const; 
    //设置系数矩阵 A 的元素 A(i,j) = val
    void SetA(int i, int j, double val);
    //设置系数矩阵 A 的所有元素，按行优先顺序传入
    void SetA(double* const arr);
    //设置常数向量 B 的元素 B(i) = val
    void SetB(int i, double val); 
    //设置常数向量 B 的所有元素，按行优先顺序传入
    void SetB(double* const arr);
    //对系数矩阵 A 进行列选主元法 LU 分解（返回 true 表示分解成功，false 表示矩阵奇异）
    bool Factorize(); 
    //对分解后的矩阵进行前向和后向替换，求解线性方程组
    void Substitute(); 
    //析构函数，释放内存
    ~LinearEquation(); 
};

inline LinearEquation::LinearEquation(int n) : N(n) {
    P = new int[n];
    A = new double[n * n];
    B = new double[n];
    X = new double[n];
    LU = new double[n * n];
    Y = new double[n];
}

inline double LinearEquation::GetA(int i, int j) const { return A[i * N + j]; }
inline double LinearEquation::GetX(int i) const { return X[i]; }
inline double LinearEquation::GetB(int i) const { return B[i]; }
inline void LinearEquation::SetA(int i, int j, double val) { A[i * N + j] = val; }
inline void LinearEquation::SetA(double* const arr) { for (int i = 0; i < N * N; i++) A[i] = arr[i]; }
inline void LinearEquation::SetB(int i, double val) { B[i] = val; }
inline void LinearEquation::SetB(double* const arr) { for (int i = 0; i < N; i++) B[i] = arr[i]; }

bool LinearEquation::Factorize() {
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
        if (pivot == 0) return false; // 矩阵奇异，无法分解
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

void LinearEquation::Substitute() {
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

LinearEquation::~LinearEquation() {
    delete[] P;
    delete[] A;
    delete[] X;
    delete[] B;
    delete[] LU;
    delete[] Y;
}

} // namespace xespice
#endif