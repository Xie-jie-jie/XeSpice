#ifndef XE_ELMVCVS_H
#define XE_ELMVCVS_H
/*
* 文件名称：xe_ElmVCVS.h
* 摘    要：电压控制电压源
* 作    者：H.J.Xie
* 完成日期：2025年8月28日
*/
#include "../xe_Circuit.h"
namespace xespice
{

struct ElmVCVS : Element {
    int N1 = -1; // 受控节点+
    int N2 = -1; // 受控节点-
    int NC1 = -1; // 控制节点+
    int NC2 = -1; // 控制节点-
    int Is = -1; // 支路电流
    double K = 0; // 比例系数

    void Create(Circuit* cir, const Vect<String>& arg) override {
        if (arg.size() < 6) {
            cir->SetError("ERR[E]001--Missing Arguments in element: " + arg[0]);
            return;
        }
        Is = cir->GetBranch(arg[0]);
        N1 = cir->GetNode(arg[1]);
        N2 = cir->GetNode(arg[2]);
        NC1 = cir->GetNode(arg[3]);
        NC2 = cir->GetNode(arg[4]);
        K = cir->GetValue(arg[5]);
        cir->Register(this, false, false); // 注册元件
    }

    void Stamp(Circuit* cir, Equation* equ, bool isOP) override {
        equ->AddA(N1, Is, 1);
        equ->AddA(N2, Is, -1);
        equ->AddA(Is, N1, 1);
        equ->AddA(Is, N2, -1);
        equ->AddA(Is, NC1, -K);
        equ->AddA(Is, NC2, K);
    }
};

} // namespace xespice
#endif // !XE_ELMVCVS_H