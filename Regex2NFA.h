#pragma once
#include<iostream>
#include"NFA.h"
using namespace std;

/// <summary>
/// 该头文件包括由正规式生成NFA的全部函数定义
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
NFA_Matrix cell2matrix(cell nfa_cell);

// 找到节点在矩阵名称中的位置，返回下标
int find_index_in_matrix(NFA_Matrix matrix, char ch);

// 重载输出matrix;
ostream& operator<<(ostream& out, const NFA_Matrix& nfa);
#pragma endregion