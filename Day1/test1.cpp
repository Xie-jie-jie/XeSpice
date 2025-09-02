#include <iostream>
#include "xe_Equation.h"

const double A[] = {
    1, 2, -1,
    2, 1, -2,
    -3, 1, 1};
const double B[] = {8, 3, 4};
const int N = 3;

int main()
{
    xespice::Equation le(N);
    le.LoadA((double*)A);
    le.LoadB((double*)B);
    le.Factorize(); // LU 分解
    le.Substitute(); // 求解线性方程组

    std::cout << "A=\n";
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) std::cout << le.GetA(i,j) << " ";
        std::cout << "\n";
    }
    std::cout << "B=\n";
    for (int i = 0; i < N; i++) std::cout << le.GetB(i) << " ";
    std::cout << "\nX=\n";
    for (int i = 0; i < N; i++) std::cout << le.GetX(i) << " ";

    system("pause"); 
    return 0;
}
