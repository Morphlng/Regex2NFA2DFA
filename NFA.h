#pragma once
#include<string>
using namespace std;

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
	char transchar;	// 转换函数 Start-transchar-End
};

// NFA单元定义
// 该结构体是NFA整体的定义，但同时也用于构造过程中的临时结点构成
struct cell
{
	edge EdgeSet[MAX];	// 边集
	int EdgeCount;	// 边数
	node Start;		// 开始节点，默认只有一个开始结点
	node End;		// 终止节点
};

// NFA矩阵结构定义
struct NFA_Matrix
{
	char* vertex;
	char** transet;
	int edgeCount;	// 边数统计
	int nodeCount;	// 节点统计
	node StartNode;	// 开始节点
	node EndNode;	// 终止节点
};