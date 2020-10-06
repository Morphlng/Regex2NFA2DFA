#pragma once
#include<iostream>
#include"NFA.h"
using namespace std;

/// <summary>
/// 该头文件本次实验的函数定义
/// 1、正规式生成NFA
/// 2、NFA确定化
/// 3、DFA最小化
/// </summary>

#pragma region 输入正规式
// 输入正规式
string input_regex();

// 检查输入的正规式是否合法
bool check_legal(string regex);

// 检查正规式是否有非法字符
bool check_character(string regex);

// 检查正规式的括号闭合
bool check_parenthesis(string regex);

// 是否是合法字符
bool is_letter(char c);

// 是否是运算符
bool is_operator(char c);
#pragma endregion

#pragma region 中缀转后缀
// 为中缀表达式添加分隔符(‘+’)
string add_separation(string regex);

// 中缀表达式转后缀表达式
string to_postfix(string regex);

// isp
int isp(char c);

// icp
int icp(char c);
#pragma endregion

#pragma region 正规式生成NFA
// 正规式生成nfa
cell regex_to_nfa(string regex);

// 初始化节点
// 主要用于根据读入字符生成一个节点
cell init_cell(char ch);

// 或运算(a|b)
cell unite_cell(cell Left, cell Right);

// 与运算(ab)
cell join_cell(cell Left, cell Right);

// 闭包运算(a*)
cell loop_cell(cell Left);

// 将c2中的边拷贝到c1
void copy_cell(cell& c1, cell c2);

// 重载输出cell;
ostream& operator<<(ostream& out, const cell& nfa);

// 将NFA_cell转为矩阵形式
Matrix cell2matrix(cell nfa_cell);

// 找到节点在矩阵名称中的位置，返回下标
int find_index_in_vertex(Matrix matrix, char ch);

// 重载输出matrix;
ostream& operator<<(ostream& out, const Matrix& nfa);
#pragma endregion

#pragma region NFA2DFA

// 将矩阵形式转换为cell形式
cell matrix2cell(Matrix nfa);

// 将NFA_cell转换为DFA_cell
Matrix NFA2DFA(Matrix nfa);

// 返回一个节点经过转换字符转换得到的结果
set<char> transfer_result(Matrix nfa, char start, char transchar);

// 返回一个节点的ε闭包
set<char> e_closure(Matrix nfa, char ch);

// 将两个集合合并
set<char> union_set(set<char>s1, set<char>s2);

// 判断两个集合是否相同
bool equal_set(set<char> s1, set<char> s2);
#pragma endregion

#pragma region DFA最小化

// DFA最小化
Matrix DFA_minimize(Matrix dfa);

// 初始化一个将节点分为“终结集”和“非终结集”的容器
vector<set<char>> init_groups(Matrix dfa);

// 从“终结集”和“非终结集”开始，进行分组
vector<set<char>> grouping(Matrix dfa, vector<set<char>> groups);

// 判断s2是否是s1子集
bool sub_set(set<char> s1, set<char> s2);
#pragma endregion
