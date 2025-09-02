#ifndef XE_ELMRESISTOR_H
#define XE_ELMRESISTOR_H
/*
* 文件名称：xe_ElmResistor.h
* 摘    要：电阻元件
* 作    者：H.J.Xie
* 完成日期：2025年8月28日
*/
#include "../xe_Circuit.h"
namespace xespice
{

struct ElmResistor : Element {
    int N1 = -1; // 节点+
    int N2 = -1; // 节点-
    double G = 0; // 电导值

    void Create(Circuit* cir, const Vect<String>& arg) override {
        if (arg.size() < 4) {
            cir->SetError("ERR[R]001--Missing Arguments in element: " + arg[0]);
            return;
        }
        N1 = cir->GetNode(arg[1]);
        N2 = cir->GetNode(arg[2]);
        G = 1.0 / cir->GetValue(arg[3]);
        cir->Register(this, false, false); // 注册元件
    }

    void Stamp(Circuit* cir, Equation* equ, bool isOP) override {
        equ->AddA(N1, N1, G);
        equ->AddA(N1, N2, -G);
        equ->AddA(N2, N1, -G);
        equ->AddA(N2, N2, G);
    }
};

} // namespace xespice
#endif // !XE_ELMRESISTOR_H