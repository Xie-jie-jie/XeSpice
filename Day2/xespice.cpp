#include "xe_Simulator.h"
#include <iostream>

int main() {
    namespace xe = xespice; // 取别名，方便使用
    xe::Circuit* cir = xe::NewCircuit(); // 创建电路
    xe::String s;
    std::cout << "Input File: ";
    std::cin >> s;
    cir->ReadFile(s); // 读取电路文件
    std::cout << "Output File: ";
    std::cin >> s;
	cir->SetOutputPath(s); // 设置输出路径
    cir->Run(); // 运行
    if (cir->ErrorFlag) std::cout << cir->ErrorMsg << std::endl;
    else std::cout << "Done." << std::endl;
    system("pause");
    delete cir;
    return 0;
}
