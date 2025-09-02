#ifndef XE_ELMCCVS_H
#define XE_ELMCCVS_H
/*
* 文件名称：xe_ElmVCVS.h
* 摘    要：电流控制电压源
* 作    者：H.J.Xie
* 完成日期：2025年8月28日
*/
#include "../xe_Circuit.h"
namespace xespice
{

struct ElmCCVS : Element {
    int N1 = -1; // 受控节点+
    int N2 = -1; // 受控节点-
    int Ix = -1; // 受控电流
    int Is = -1; // 支路电流
    double K = 0; // 比例系数

    void Create(Circuit* cir, const Vect<String>& arg) override {
        if (arg.size() < 5) {
            cir->SetError("ERR[H]001--Missing Arguments in element: " + arg[0]);
            return;
        }
        Is = cir->GetBranch(arg[0]);
        N1 = cir->GetNode(arg[1]);
        N2 = cir->GetNode(arg[2]);
        Ix = cir->GetBranch(arg[3]);
        K = cir->GetValue(arg[4]);
        cir->Register(this, false, false); // 注册元件
    }

    void Stamp(Circuit* cir, Equation* equ, bool isOP) override {
        equ->AddA(N1, Is, 1);
        equ->AddA(N2, Is, -1);
        equ->AddA(Is, N1, 1);
        equ->AddA(Is, N2, -1);
        equ->AddA(Is, Ix, -K);
    }
};

} // namespace xespice
#endif // !XE_ELMCCVS_H