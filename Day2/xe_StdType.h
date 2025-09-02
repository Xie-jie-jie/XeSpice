#ifndef XE_STDTYPE_H
#define XE_STDTYPE_H
/*
* 文件名称：xe_StdType.h
* 摘    要：XeSpice 项目用到的标准库和 STL 容器
* 作    者：H.J.Xie
* 完成日期：2025年8月26日
*/
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <fstream>
#include <cctype>
#include <cmath>
#include <iomanip>
namespace xespice 
{
    // 字符串类型
    using String = std::string;
    // 泛型动态数组类型
    template<typename T>
    using Vect = std::vector<T>;
    // 泛型字典类型
    template<typename T>
    using Dict = std::map<String, T>;
    // 泛型集合类型
    template<typename T>
    using HashSet = std::unordered_set<T>;
    // 文件输入流
    using InStream = std::ifstream;
    // 文件输出流
    using OutStream = std::ofstream;
}
#endif //!XE_STDTYPE_H