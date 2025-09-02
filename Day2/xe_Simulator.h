#ifndef XE_SIMULATOR_H
#define XE_SIMULATOR_H
/*
* 文件名称：xe_SIMULATOR.h
* 摘    要：电路仿真程序顶层架构
* 作    者：H.J.Xie
* 完成日期：2025年8月29日
*/
#include "xe_Circuit.h"
#include "element/xe_ElmResistor.h"
#include "element/xe_ElmVoltageSource.h"
#include "element/xe_ElmCurrentSource.h"
#include "element/xe_ElmVCVS.h"
#include "element/xe_ElmVCCS.h"
#include "element/xe_ElmCCVS.h"
#include "element/xe_ElmCCCS.h"
namespace xespice 
{

// 元器件构造函数
static inline Element* NewElement(char ch) {
    switch (ch) {
    case 'r': return new ElmResistor();
    case 'v': return new ElmVoltageSource();
    case 'i': return new ElmCurrentSource();
    case 'e': return new ElmVCVS();
    case 'g': return new ElmVCCS();
    case 'h': return new ElmCCVS();
    case 'f': return new ElmCCCS();
    default: return nullptr;
    }
}

// 供外部调用
static inline Circuit* NewCircuit() {
    Circuit* cir = new Circuit();
    cir->SetElementCtor(NewElement);
    return cir; 
}

}
#endif // !XE_SIMULATOR_H