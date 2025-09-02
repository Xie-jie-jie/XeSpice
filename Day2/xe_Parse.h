#ifndef XE_PARSE_H
#define XE_PARSE_H
/*
* 文件名称：xe_Parse.h
* 摘    要：用于语法分析的通用工具函数
* 作    者：H.J.Xie
* 完成日期：2025年8月27日
*/
#include "xe_StdType.h"
namespace xespice 
{
// 将字符串按空格分解
static inline void Str_Split(Vect<String>& res, const String& s) {
    String token = "";
    for (char ch : s) {
        if (ch == ' ') {
            if (token.size() > 0) res.push_back(token);
            token = "";
        }
        else token += std::tolower(ch);
    }
}
// 将字符串字母转小写
static inline String Str_ToLower(const String& s) {
    String res = "";
    for (char ch : s) res += std::tolower(ch);
    return res;
}
// 将字符串转为数值（返回 NaN 表示错误）
static inline double Str_ToValue(const String& s) {
    String state = "";
    String numstr = "";
    bool isSci = false; // 是否为科学计数法
    // 分割数字和数量级单位
    for (char ch : s) {
        if (std::isdigit(ch) || ch == '+' || ch == '-' || ch == '.') {
            if (state == "") numstr += ch;
            else if (state == "e" && !isSci) {
                isSci = true;
                state = "";
                numstr += 'e';
                numstr += ch;
            }
            else break;
        }
        else if (std::isalpha(ch)) {
            char c = std::tolower(ch); // 转小写
            if (state == "") {
                if (c == 't') { state = "t"; break; }
                else if (c == 'g') { state = "g"; break; }
                else if (c == 'k') { state = "k"; break; }
                else if (c == 'u') { state = "u"; break; }
                else if (c == 'n') { state = "n"; break; }
                else if (c == 'p') { state = "p"; break; }
                else if (c == 'f') { state = "f"; break; }
                else if (c == 'e') { state = "e"; }
                else if (c == 'm') { state = "m"; }
                else break;
            }
            else if (state == "m") {
                if (c == 'e') { state = "me"; }
                else if (c == 'i') { state = "mi"; }
                else break;
            }
            else if (state == "me") {
                if (c == 'g') { state = "M"; break; }
                else break;
            }
            else if (state == "mi") {
                if (c == 'l') { state = "l"; break; }
                else break;
            }
            else break;
        }
        else break;
    }
    // 识别数量级单位
    double scale = 1;
    if (state.size() > 0) {
        switch (state[0]) {
        case 't': scale = 1e12; break;
        case 'g': scale = 1e9; break;
        case 'M': scale = 1e6; break;
        case 'k': scale = 1e3; break;
        case 'm': scale = 1e-3; break;
        case 'l': scale = 25.4e-6; break;
        case 'u': scale = 1e-6; break;
        case 'n': scale = 1e-9; break;
        case 'p': scale = 1e-12; break;
        case 'f': scale = 1e-15; break;
        default: break;
        }
    }
    // 识别数字
    double number;
    try {
        number = std::stod(numstr) * scale;
    }
    catch (...) {
        number = std::nan("");
    }
    return number;
}

}
#endif // !XE_PARSE_H