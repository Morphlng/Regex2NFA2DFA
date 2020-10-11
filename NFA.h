#pragma once
#include<string>
#include<set>
#include<vector>
using namespace std;

/// <summary>
/// 该头文件包含了NFA与DFA的结构体定义
/// </summary>

#define MAX 100

// 节点定义
struct node
{
	char Name;
};

// 边定义
struct edge
{
	node Start;
	node End;
	char transchar;		// 转换函数 Start-transchar-End
};

// NFA与DFA单元定义
// 该结构体是NFA整体的定义，但同时也用于构造过程中的临时结点构成
struct cell
{
	edge EdgeSet[MAX];		// 边集
	int EdgeCount;			// 边数
	set<char> transchar;	// 转换函数名称集 (a,b,c...)
	node StartNode;			// 开始节点，默认只有一个开始结点
	vector<node> EndNodeSet;	// 终止节点集，对于Thompson算法生成的NFA只有一个终止结点，但是DFA会有多个
};

// NFA与DFA的矩阵结构定义
struct Matrix
{
	char* vertex;			// 节点名称，其大小为vertex[nodeCount]
	set<char> transchar;	// 转换函数名称集 (a,b,c...)
	string** transet;			// 转换矩阵，其大小为transet[nodeCount][nodeCount]
	int edgeCount;			// 边数统计
	int nodeCount;			// 节点统计
	node StartNode;			// 开始节点
	vector<node> EndNodeSet;	// 终止节点集
};