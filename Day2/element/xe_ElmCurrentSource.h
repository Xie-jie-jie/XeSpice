#ifndef XE_ELMCURRENTSOURCE_H
#define XE_ELMCURRENTSOURCE_H
/*
* 文件名称：xe_ElmCurrentSource.h
* 摘    要：电流源
* 作    者：H.J.Xie
* 完成日期：2025年8月28日
*/
#include "../xe_Circuit.h"
namespace xespice
{

struct ElmCurrentSource : Element {
    int N1 = -1; // 节点+
    int N2 = -1; // 节点-
    double Idc = 0; // 直流电流

    void Create(Circuit* cir, const Vect<String>& arg) override {
        if (arg.size() < 4) {
            cir->SetError("ERR[I]001--Missing Arguments in element: " + arg[0]);
            return;
        }
        N1 = cir->GetNode(arg[1]);
        N2 = cir->GetNode(arg[2]);
        if (arg.size() > 4) {
            if (arg[3] == "DC") Idc = cir->GetValue(arg[4]);
            else {
                cir->SetError("ERR[I]002--Invalid Argument in element: " + arg[0]);
                return;
            }
        }
        else Idc = cir->GetValue(arg[3]);
        cir->Register(this, false, false); // 注册元件
    }

    void Stamp(Circuit* cir, Equation* equ, bool isOP) override {
        equ->AddB(N1, -Idc);
        equ->AddB(N2, Idc);
    }
};

} // namespace xespice
#endif // !XE_ELMCURRENTSOURCE_H