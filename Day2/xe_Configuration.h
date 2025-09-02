#ifndef XE_CONFIGURATION_H
#define XE_CONFIGURATION_H
/*
* 文件名称：xe_Configuration.h
* 摘    要：电路仿真程序的配置参数
* 作    者：H.J.Xie
* 完成日期：2025年8月24日
*/
namespace xespice 
{
// 配置参数结构体
struct Configuration {
    /*//////////////////// 通用设置 ////////////////////*/
    int NUMDGT = 6; // 输出结果的有效数字位数（0~15）
    double PIVTOL = 1e-13; // 矩阵中可被接受为主元的最小值（Pivot Tolerance）
    double GMIN = 1e-12; // 各节点到地的附加电导
};

}
#endif // !XE_OPTION_H
