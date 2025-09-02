#ifndef XE_CIRCUIT_H
#define XE_CIRCUIT_H
/*
* 文件名称：xe_Circuit.h
* 摘    要：电路类
* 作    者：H.J.Xie
* 完成日期：2025年8月26日
*/
#include "xe_StdType.h"
#include "xe_Equation.h"
#include "xe_Configuration.h"
#include "xe_Parse.h"
namespace xespice
{

struct Circuit; // 电路类（前向声明）
// 电路元件类（抽象类）
struct Element {
    // 根据字符串列表创建元件
    virtual void Create(Circuit* cir, const Vect<String>& arg) = 0;
    // 元件 stamp 到 MNA 方程（isOP 表示是否为直流工作点分析）
    virtual void Stamp(Circuit* cir, Equation* equ, bool isOP) = 0;
    // 析构函数
    virtual ~Element() {};
};
// 电路元件构造函数（由小写字母指定电路元件类型）
using ElementCtor = Element*(*)(char ch);
// 电路类
struct Circuit {
public:
    /*//////////////////// 公共成员变量 ////////////////////*/
    Configuration Config; // 电路配置参数
    bool ErrorFlag = false; // 电路是否有错误
    String ErrorMsg = ""; // 错误信息
    /*//////////////////// 供外部使用 ////////////////////*/
    // 设置电路元件构造函数函数
    void SetElementCtor(ElementCtor ctor);
    // 设置输出路径（true 表示成功）
    bool SetOutputPath(const String& filepath);
    // 读取网表文件，构建电路（返回 true 表示成功）
    // isLib 表示是否为库文件
    bool ReadFile(const String& filepath, bool isLib=false);
    // 运行仿真程序（返回 true 表示仿真成功）
    bool Run();
    /*//////////////////// 供 Element 类使用 ////////////////////*/
    // 查找节点电压，若不存在则创建，返回节点编号（不区分大小写）
    int GetNode(const String& name);
    // 查找支路电流，若不存在则创建，返回支路编号（不区分大小写）
    int GetBranch(const String& name);
    // 创建一个辅助变量（在解向量中），返回编号
    int NewAux();
    // 注册元件，提供信息：是否为动态，是否为非线性
    void Register(Element* elm, bool isDynamic, bool isNonlinear);
    /*//////////////////// 通用 ////////////////////*/
    // 设置错误信息
    void SetError(const String& msg);
    // 将字符串转为数值
    double GetValue(const String& s);
    // 构造函数
    Circuit();
    // 析构函数
    ~Circuit();
private:
    /*//////////////////// 私有成员变量 ////////////////////*/
    String Title = ""; // 电路文件标题
    String DirPath = ""; // 文件目录路径 
    OutStream OutputFile; // 输出文件
    ElementCtor ElmCtor = nullptr; // 电路元件构造函数
    /*//////////////////// 电路方程相关 ////////////////////*/
    int Xsize = 0; // 解向量规模
    Equation* MNA = nullptr; // MNA 方程
    Dict<int> NodeDict; // 电路节点电压编号字典
    Dict<int> BranchDict; // 电路支路电流编号字典
    Dict<Element*> ElmDict; // 电路元件字典
    HashSet<Element*> FixedSet; // 固定元件集合
    /*//////////////////// 主电路描述 ////////////////////*/
    Vect<String> ElementMemo; // 元件描述存储
    /*//////////////////// 内部函数 ////////////////////*/
    // 读取主电路标题
    void ReadTitle(InStream& file);
    // 读取经过精简后的一行
    void ReadLine(const String& line);
    // 读取命令
    void ReadCommand(const String& line);
    // 创建元件
    void CreateElement(Vect<String>& elementMemo);
    // 创建元件
    void RunOP();
    // 输出 OP 分析后的节点电压和支路电流
    void PrintOP();
    // 执行 .OPTIONS 命令
    void CmdOptions(const Vect<String>& tokens);
};

inline void Circuit::SetElementCtor(ElementCtor ctor) {
    ElmCtor = ctor;
}

inline bool Circuit::SetOutputPath(const String& filepath) {
    if (OutputFile.is_open()) OutputFile.close();
    OutputFile.open(filepath, std::ios::trunc);
    if (!OutputFile) {
        SetError("ERR001--Cannot open output file: " + filepath);
        return false;
    }
    return true;
}

inline bool Circuit::ReadFile(const String& filepath, bool isLib)
{
    if (ErrorFlag) return false;
    InStream infile(filepath);
    if (!infile) {
        SetError("ERR002--Cannot open netlist file: " + filepath);
        return false;
    }
    String blankNormal = " \t\f\v\r\n,=()[]"; // 普通模式中的空白符
    String blankExpr = " \t\f\v\r\n"; // 表达式中的空白符
    char ch = ' '; // 读取的字符
    char chPrev = ' '; // 前一个字符
    String line = "";
    bool lineEnd = false; // 换行状态
    int braceCount = 0; // 大括号计数
    // 判断是否为主文件
    if (!isLib) {
        ReadTitle(infile); // 读取标题
        DirPath = ""; // 提取目录部分（以 '/' 结尾）
        size_t pos = filepath.find_last_of("/\\");
        if (pos != std::string::npos) DirPath = filepath.substr(0, pos+1);
    }
    // 读取内容（将空白符替换为空格）
    while (infile.get(ch)) {
        if (lineEnd) { // 换行时的判断
            if (ch != '+') { // 若非续行符
                ReadLine(line);
                line = "";
                braceCount = 0; // 恢复到初始状态
            }
            else ch = ' '; // 续行符'+'替换为空格
            lineEnd = false; // 重置换行状态
        }
        if (ch == '\n') lineEnd = true; // 若为换行符，设置换行状态
        if (braceCount <= 0) { // 普通状态，消除连续的空白符
            // 将空白字符替换为空格
            if (blankNormal.find(ch) != std::string::npos) ch = ' ';
            // 如果不为连续空格，则添加到字符串中
            if (!(chPrev == ' ' && ch == ' ' )) line += ch;
        }
        else { // 表达式状态，忽略空白符
            // 将空白字符替换为空格
            if (blankExpr.find(ch) != std::string::npos) ch = ' ';
            // 不为空白符则添加到字符串中
            if (ch != ' ') line += ch;
        }
        if (ch == '{') braceCount += 1; // 如果为左大括号，计数加一
        if (ch == '}') braceCount -= 1; // 如果为右大括号，计数减一
        chPrev = ch; // 上一个字符更新
    }
    if (ErrorFlag) return false;
    else return true;
}

inline bool Circuit::Run() {
    if (ErrorFlag) return false;
    CreateElement(ElementMemo); // 构建主电路
    if (ErrorFlag) return false;
    MNA = new Equation(Xsize); // 构建 MNA 方程
    for (const auto& pair : NodeDict) { // 添加节点到地的附加电导
        MNA->AddA(pair.second, pair.second, Config.GMIN);
    }
    RunOP(); // 运行直流工作点分析
    PrintOP(); // 输出 .OP 结果
    OutputFile.close();
    return ErrorFlag;
}

inline int Circuit::GetNode(const String& name) {
    String s = Str_ToLower(name);
    if (NodeDict.find(s) == NodeDict.end()) {
        NodeDict[s] = Xsize;
        Xsize++;
        return Xsize-1;
    }
    else return NodeDict[s];
}

inline int Circuit::GetBranch(const String& name) {
    String s = Str_ToLower(name);
    if (BranchDict.find(s) == BranchDict.end()) {
        BranchDict[s] = Xsize;
        Xsize++;
        return Xsize-1;
    }
    else return BranchDict[s];
}

inline int Circuit::NewAux() {
    Xsize++;
    return Xsize-1;
}

inline void Circuit::Register(Element* elm, bool isDynamic, bool isNonlinear) {
    if (!isDynamic && !isNonlinear) {
        FixedSet.emplace(elm); // 加入固定元件集合，这些元件只用 Stamp 一次
    }
    // 其余的以后再来探索吧~
}

inline void Circuit::SetError(const String& msg)
{
    if (!ErrorFlag) {
        ErrorFlag = true;
        ErrorMsg = msg;
    }
}

inline double Circuit::GetValue(const String& s) {
    double num = Str_ToValue(s);
    if (std::isnan(num)) SetError("ERR004--Invalid Value: " + s);
    return num;
}

inline void Circuit::ReadTitle(InStream& file) {
    char ch = ' ';
    String line = "";
    // 读取电路标题
    while (file.get(ch)) {
        if (ch == '\n') break;
        else if (ch != '\r') line += ch;
    }
    Title = line;
}

inline void Circuit::ReadLine(const String& line) {
    // 跳过空字符串和注释行（以 '*'开头）
    if (line.size() < 1 || line[0] == '*') return;
    char headChar = std::tolower(line[0]); // 行开头字母
    if (headChar == '.') ReadCommand(line); // 处理'.'开头的控制语句
    else if (headChar >= 'a' && headChar <= 'z') { // 处理元件描述
        ElementMemo.push_back(line); // 将描述放入主电路
    } 
    else SetError("ERR003--Invalid Line: " + line);
}

inline void Circuit::ReadCommand(const String& line) {
    // 分割行内容为 tokens（以空格分隔）
    Vect<String> tokens;
    Str_Split(tokens, line);
    String cmd = Str_ToLower(tokens[0].substr(1)); // 转换为小写便于比较
    // 执行指令
    if (cmd == "op" || cmd == "end") return; // 这两个命令我们不需要操作
    else if (cmd == "options") CmdOptions(tokens);
    else SetError("ERR005--Unrecognizable Command: " + tokens[0]);
}

inline void Circuit::CreateElement(Vect<String>& elementMemo) {
    if (ErrorFlag) return;
    for (String& s : elementMemo) {
        Vect<String> arg;
        Str_Split(arg, s);
        Element* ptr = ElmCtor(std::tolower(arg[0][0]));
        if (ptr == nullptr) {
            SetError("ERR008--Element Construction Failed: " + arg[0]);
            return;
        }
        ptr->Create(this, arg);
        ElmDict.emplace(arg[0], ptr);
        if (ErrorFlag) return;
    }
}

inline void Circuit::RunOP() {
    if (ErrorFlag) return;
    for (Element* elm : FixedSet) {
        elm->Stamp(this, MNA, true);
    }
    if (MNA->Factorize(Config.PIVTOL)) MNA->Substitute();
    else SetError("ERR009--Singular Matrix!");
}

inline void Circuit::PrintOP() {
    if (ErrorFlag) return;
    for (const auto& pair : NodeDict) {
        OutputFile << "V(" << pair.first << ")\t";
        OutputFile << std::scientific << std::setprecision(Config.NUMDGT) 
        << MNA->GetX(pair.second) << std::endl;
    }
    for (const auto& pair : BranchDict) {
        OutputFile << "I(" << pair.first << ")\t";
        OutputFile << std::scientific << std::setprecision(Config.NUMDGT) 
        << MNA->GetX(pair.second) << std::endl;
    }
}

inline void Circuit::CmdOptions(const Vect<String>& tokens) {
    if (tokens.size() % 2 == 0) {
        SetError("ERR006--Missing .OPTIONS Arguments!");
        return;
    }
    for (int i = 1; i < tokens.size(); i+=2) { // 两个一组
        String s = Str_ToLower(tokens[i]);
        if (s == "numdgt") {
            int n = GetValue(tokens[i+1]);
            Config.NUMDGT = n & 0xf; // 将 n 限定在 0~15
        }
        else if (s == "pivtol") Config.PIVTOL = GetValue(tokens[i+1]);
        else if (s == "gmin") Config.GMIN = GetValue(tokens[i+1]);
        else {
            SetError("ERR007--Unknown Option: " + tokens[i]);
            return;
        }
    }
}

Circuit::Circuit() {
    NodeDict["0"] = -1;
}

Circuit::~Circuit() {
    // 释放元件
    for (auto iter = ElmDict.begin(); iter != ElmDict.end(); iter++) {
        delete iter->second;
    }
}

} // namespace xespice
#endif // !XE_CIRCUIT_H