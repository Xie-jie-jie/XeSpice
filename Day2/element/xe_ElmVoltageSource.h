#ifndef XE_ELMVOLTAGESOURCE_H
#define XE_ELMVOLTAGESOURCE_H
/*
* 文件名称：xe_ElmVoltageSource.h
* 摘    要：电压源
* 作    者：H.J.Xie
* 完成日期：2025年8月28日
*/
#include "../xe_Circuit.h"
namespace xespice
{

struct ElmVoltageSource : Element {
    int N1 = -1; // 节点+
    int N2 = -1; // 节点-
    int Is = -1; // 支路电流
    double Vdc = 0; // 直流电压

    void Create(Circuit* cir, const Vect<String>& arg) override {
        if (arg.size() < 4) {
            cir->SetError("ERR[V]001--Missing Arguments in element: " + arg[0]);
            return;
        }
        N1 = cir->GetNode(arg[1]);
        N2 = cir->GetNode(arg[2]);
        Is = cir->GetBranch(arg[0]);
        if (arg.size() > 4) {
            if (arg[3] == "DC") Vdc = cir->GetValue(arg[4]);
            else {
                cir->SetError("ERR[V]002--Invalid Argument in element: " + arg[0]);
                return;
            }
        }
        else Vdc = cir->GetValue(arg[3]);
        cir->Register(this, false, false); // 注册元件
    }

    void Stamp(Circuit* cir, Equation* equ, bool isOP) override {
        equ->AddA(N1, Is, 1);
        equ->AddA(N2, Is, -1);
        equ->AddA(Is, N1, 1);
        equ->AddA(Is, N2, -1);
        equ->AddB(Is, Vdc);
    }
};

} // namespace xespice
#endif // !XE_ELMVOLTAGESOURCE_H