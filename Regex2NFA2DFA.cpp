#include"Regex2NFA2DFA.h"
#include<stack>
#include<vector>
#include<queue>
using namespace std;

/// <summary>
/// 该源文件实现了实验一的函数
/// 1、正规式生成NFA
/// 2、NFA确定化
/// 3、DFA最小化
/// </summary>

#pragma region 输入正规式

// 输入正规式
string input_regex()
{
	string regex;
	cout << "请输入正则表达式：  （操作符：() * |;字符集：a~z A~Z）" << endl;
	do
	{
		cin >> regex;
	} while (!check_legal(regex));

	return regex;
}

// 检查输入的正规式是否合法
bool check_legal(string regex)
{
	if (check_character(regex) && check_parenthesis(regex))
	{
		return true;
	}
	return false;
}

// 检查正规式是否有非法字符
bool check_character(string regex)
{
	int i;
	for (i = 0; i < regex.length(); i++)
	{
		if (is_letter(regex[i]))
		{
			// cout << "是字符" << endl;
		}
		else if (is_operator(regex[i]))
		{
			// cout << "是符号" << endl;
		}
		else
		{
			cerr << "输入含不合法的字符，请重新输入！" << endl;
			return false;
		}
	}
	return true;
}

// 是否是合法字符
bool is_letter(char c)
{
	if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
	{
		return true;
	}
	return false;
}

// 是否是运算符
bool is_operator(char c)
{
	if (c == '(' || c == ')' || c == '*' || c == '|' || c == '+')
	{
		return true;
	}
	return false;
}

// 检查正规式的括号闭合
bool check_parenthesis(string regex)
{
	stack<int> s;
	int i;
	for (i = 0; i < regex.length(); i++)
	{
		if (regex[i] == '(')
		{
			s.push(1);
		}
		else if (regex[i] == ')')
		{
			if (s.empty())
			{
				cerr << "有多余的右括号，请重新输入！" << endl;
				return false;
			}
			else
				s.pop();
		}
	}
	if (!s.empty())
	{
		cerr << "有多余的左括号，请重新输入！" << endl;
		return false;
	}
	return true;
}

#pragma endregion

#pragma region 运算符优先级定义

/*
构造优先级表规则：（1）先括号内，再括号外；（2）优先级由高到低：闭包、|、+；（3）同级别，先左后右。
优先级表：
	 #	(	*	|	+	)
isp  0	1	7	5	3	8
icp	 0	8	6	4	2	1

isp: in stack priority，栈内优先级
icp：in coming priority，入栈优先级
*/

/*********************运算符优先级关系表********************/
/*
	c1\c2	(	*	|	+	)	#

	(		<	<	<	<	=	>

	*		<	>	>	>	>	>

	|		<	<	>	>	>	>

	)       =	>	>	>	>	>

	+		<	<	<	>	>	>

	#		<	<	<	<	<	=
*/

// 上表意思为：c1 _(<|>|=) c2

/***********************************************************/

#pragma endregion

#pragma region 中缀转后缀

// 举例：
// 中缀：a(a|b)*((ab)|c)*
// 对中缀做分隔处理：a+(a|b)*+((a+b)|c)*，方便改为后缀表达式（同时也消解了(ab)的歧义问题）
// 后缀：aab|*+ab+c|*+

// 为中缀表达式添加分隔符(‘+’)
string add_separation(string regex)
{
	char* result = new char[2 * regex.length() + 1];
	int i, index = 0;
	char c, p;		// current character, previous character
	for (i = 1; i < regex.length(); i++)
	{
		p = regex[i-1];
		c = regex[i];
		result[index++] = p;
		// 下面开始分情况添加‘+’
		// 1、若c是字母、p不是'('、'|'都要添加
		if (p != '(' && p != '|' && is_letter(c))
		{
			result[index++] = '+';
		}
		// 2、若c是'(',p不是'|'、'(',也要加
		else if (c == '(' && p != '|' && p != '(')
		{
			result[index++] = '+';
		}
	}
	result[index++] = c;	// 把最后一个字符加上
	result[index] = '\0';
	return string(result);
}

// 中缀表达式转后缀表达式 
string to_postfix(string regex)
{
	regex += "#";						// 给待转换中缀表达式加入终止符'#'
	int i = 0, index = 0, count = 0;	// count的作用是在结尾加入'\0'
	char ch = regex[index++], op;
	char* result = new char[regex.length()];	// 结果字符串
	stack<char> s;
	s.push('#');	// 开始符入栈
	while (!s.empty())
	{
		if (is_letter(ch))
		{
			result[i++] = ch;
			ch = regex[index++];
			count++;
		}
		else
		{
			op = s.top();
			if (isp(op) < icp(ch))			// 如果栈内优先级小于入栈优先级，则进栈
			{
				s.push(ch);
				ch = regex[index++];
			}
			else if (isp(op) > icp(ch))		// 如果栈内优先级大于入栈优先级，则退栈并加入结果
			{
				result[i++] = op;
				count++;
				s.pop();
			}
			else                            // 如果栈内优先级等于入栈优先级，则退栈，但不加入结果
			{
				s.pop();
				// 优先级相等的情况只有'#'-'#'，'('-')'，而只有栈顶为'('读入')'时才不是终止状态
				if (op == '(')
				{
					ch = regex[index++];
				}
			}
		}
	}
	result[count] = '\0';
	return string(result);
}

// isp
int isp(char c)
{
	switch (c)
	{
		case '#': return 0;
		case '(': return 1;
		case '*': return 7;
		case '|': return 5;
		case '+': return 3;
		case ')': return 8;
		default:
			break;
	}
	cerr << "isp error" << endl;
	return -1;
}

// icp
int icp(char c)
{
	switch (c)
	{
		case '#': return 0;
		case '(': return 8;
		case '*': return 6;
		case '|': return 4;
		case '+': return 2;
		case ')': return 1;
	}
	cerr << "icp error!" << endl;
	return -1;
}

#pragma endregion

#pragma region 正规式-NFA

// 采用Thompson算法构建整个NFA，
// 对不同的操作符，需要不同数量的运算数，因此a|b、ab、a*、a操作需分别用函数实现

char STATE_NAME = 65;	// 定义NFA中节点的名称，从A开始

cell regex_to_nfa(string regex)
{
	int i, flag = 0;
	char ch;
	set<char> transchar;		// 记录转换字符 
	cell NFA, Cell, Left, Right;
	// NFA：最终结果
	// Cell：临时变量
	// Left：左操作符
	// Right：右操作符
	stack<cell> s;
	for (i = 0; i < regex.length(); i++)
	{
		ch = regex[i];
		switch (ch)
		{
		case '|':
		{
			Right = s.top();
			s.pop();
			Left = s.top();
			s.pop();
			Cell = unite_cell(Left, Right);
			s.push(Cell);
			break;
		}
		case '*':
		{
			Left = s.top();
			s.pop();
			Cell = loop_cell(Left);
			s.push(Cell);
			break;
		}
		case '+':
		{
			Right = s.top();
			s.pop();
			Left = s.top();
			s.pop();
			Cell = join_cell(Left, Right);
			s.push(Cell);
			break;
		}
		default:
			Cell = init_cell(ch);
			s.push(Cell);
			if (ch != '#')
				transchar.insert(ch);	// set会首先检测是否存在，只有当不存在时才会插入
			break;
		}
	}
	NFA = s.top();
	s.pop();
	NFA.transchar = transchar;
	STATE_NAME = 65;	// 将计数器重置，方便再次调用
	return NFA;
}

// 初始化节点
cell init_cell(char ch)
{
	edge newEdge;
	newEdge.Start.Name = STATE_NAME++;
	newEdge.End.Name = STATE_NAME++;
	newEdge.transchar = ch;
	
	cell newCell;
	newCell.EdgeCount = 0;
	newCell.EdgeSet[newCell.EdgeCount++] = newEdge;
	newCell.StartNode = newEdge.Start;
	newCell.EndNodeSet.push_back(newEdge.End);
	return newCell;
}

// 或运算(a|b)
// 一共需要添加两个节点，四条边
// 新开始节点指向Left/Right开始节点，Left/Right终止节点指向新终止节点
cell unite_cell(cell Left, cell Right)
{
	cell newCell;
	newCell.EdgeCount = 0;
	newCell.StartNode.Name = STATE_NAME++;
	node newEnd;
	newEnd.Name = STATE_NAME++;
	newCell.EndNodeSet.push_back(newEnd);

	edge edge1, edge2, edge3, edge4;
	// 新开始节点指向Left开始节点
	edge1.Start = newCell.StartNode;
	edge1.End = Left.StartNode;
	edge1.transchar = '#';

	// 新开始节点指向Right开始节点
	edge2.Start = newCell.StartNode;
	edge2.End = Right.StartNode;
	edge2.transchar = '#';

	// Left终止节点指向新终止节点
	edge3.Start = Left.EndNodeSet[0];
	edge3.End = newCell.EndNodeSet[0];
	edge3.transchar = '#';

	// Left终止节点指向新终止节点
	edge4.Start = Right.EndNodeSet[0];
	edge4.End = newCell.EndNodeSet[0];
	edge4.transchar = '#';

	// 将Left和Right的EdgeSet复制到NewCell
	copy_cell(newCell, Left);
	copy_cell(newCell, Right);

	// 将新增的边加入
	newCell.EdgeSet[newCell.EdgeCount++] = edge1;
	newCell.EdgeSet[newCell.EdgeCount++] = edge2;
	newCell.EdgeSet[newCell.EdgeCount++] = edge3;
	newCell.EdgeSet[newCell.EdgeCount++] = edge4;

	return newCell;
}

// 与运算(a+b)
// Left的结束状态与Right的开始状态合并
cell join_cell(cell Left, cell Right)
{
	// 我们需要将四个节点合并成三个(A-a-B，C-b-D → A-a-B-b-D)
	// 即Left的结束状态和Right的开始状态合并，并将Right中其他的边复制给Left，最后返回Left
	
	int i;
	// 首先做合并处理
	for (i = 0; i < Right.EdgeCount; i++)
	{
		// Right的开始状态与Left的结束状态合并
		if (Right.EdgeSet[i].Start.Name == Right.StartNode.Name)
		{
			Right.EdgeSet[i].Start = Left.EndNodeSet[0];
			// STATE_NAME--;
		}
		else if (Right.EdgeSet[i].End.Name == Right.StartNode.Name)
		{
			Right.EdgeSet[i].End = Left.EndNodeSet[0];
			// STATE_NAME--;
		}

		// 将Right的终止结点前移
		if (Right.EdgeSet[i].End.Name == Right.EndNodeSet[0].Name)
		{
			Right.EdgeSet[i].End.Name -= 1;
		}
		if (Right.EdgeSet[i].Start.Name == Right.EndNodeSet[0].Name)
		{
			Right.EdgeSet[i].Start.Name -= 1;
		}
	}
	Right.StartNode = Left.EndNodeSet[0];
	Right.EndNodeSet[0].Name -= 1;
	STATE_NAME--;

	// 然后做边复制的操作
	copy_cell(Left, Right);

	Left.EndNodeSet = Right.EndNodeSet;
	return Left;
}

// 闭包运算(a*)
// 一共需要添加两个节点，四条边。
// 新开始节点指向原开始结点，原终止结点指向新终止节点，原终止节点指向原开始节点的边，新开始结点指向新终止节点
cell loop_cell(cell Left)
{
	cell newCell;
	newCell.EdgeCount = 0;
	newCell.StartNode.Name = STATE_NAME++;
	node newEnd;
	newEnd.Name = STATE_NAME++;
	newCell.EndNodeSet.push_back(newEnd);

	edge edge1, edge2, edge3, edge4;
	// 新开始节点指向原开始结点
	edge1.Start = newCell.StartNode;
	edge1.End = Left.StartNode;
	edge1.transchar = '#';

	// 原终止结点指向新终止节点
	edge2.Start = Left.EndNodeSet[0];
	edge2.End = newCell.EndNodeSet[0];
	edge2.transchar = '#';

	// 原终止节点指向原开始节点的边
	edge3.Start = Left.EndNodeSet[0];
	edge3.End = Left.StartNode;
	edge3.transchar = '#';

	// 新开始结点指向新终止节点
	edge4.Start = newCell.StartNode;
	edge4.End = newCell.EndNodeSet[0];
	edge4.transchar = '#';

	// 将原来的边复制过来
	copy_cell(newCell, Left);
	// 添加新增的四条边
	newCell.EdgeSet[newCell.EdgeCount++] = edge1;
	newCell.EdgeSet[newCell.EdgeCount++] = edge2;
	newCell.EdgeSet[newCell.EdgeCount++] = edge3;
	newCell.EdgeSet[newCell.EdgeCount++] = edge4;

	return newCell;
}

// 将c2中的边拷贝到c1
void copy_cell(cell &c1, cell c2)
{
	int i;
	for (i = 0; i < c2.EdgeCount; i++)
	{
		c1.EdgeSet[c1.EdgeCount + i] = c2.EdgeSet[i];
	}
	c1.EdgeCount += c2.EdgeCount;
}

// 重载输出cell
ostream& operator<<(ostream& out, const cell& nfa)
{
	int i;
	out << "自动机M 的边数：" << nfa.EdgeCount << endl;
	out << "自动机M 的起始状态：" << nfa.StartNode.Name << endl;
	out << "自动机M 的结束状态：";
	vector<node>::const_iterator node_iter;
	for (node_iter = nfa.EndNodeSet.begin(); node_iter != nfa.EndNodeSet.end(); node_iter++)
	{
		out << (*node_iter).Name << " ";
	}
	out << endl;
	out << "自动机M 的转换字符集有:";
	set<char>::iterator transchar_iter;
	for (transchar_iter = nfa.transchar.begin(); transchar_iter != nfa.transchar.end(); transchar_iter++)
	{
		out << *transchar_iter << " ";
	}
	out << endl << endl;

	for (i = 0; i < nfa.EdgeCount; i++)
	{
		out << "第" << i + 1 << "条边的起始状态：" << nfa.EdgeSet[i].Start.Name
			<< "  结束状态：" << nfa.EdgeSet[i].End.Name
			<< "  转换符：" << nfa.EdgeSet[i].transchar << endl;
	}
	out << "结束" << endl;

	return out;
}

// 将cell转为矩阵形式
Matrix cell2matrix(cell nfa_cell)
{
	int i, j;
	Matrix matrix;
	matrix.edgeCount = 0;
	vector<node>::iterator end_iter = nfa_cell.EndNodeSet.begin();
	int maxName = (*end_iter).Name;
	for (++end_iter; end_iter != nfa_cell.EndNodeSet.end(); end_iter++)
	{
		if ((*end_iter).Name > maxName)
		{
			maxName = (*end_iter).Name;
		}
	}

	matrix.nodeCount = maxName - 65 + 1;	// 统计字母数量
	
	// 初始化矩阵节点名称
	matrix.vertex = new char[matrix.nodeCount];
	for (i = matrix.nodeCount - 1, j = 65; i >= 0; i--,j--)
	{
		matrix.vertex[i] = char(j + matrix.nodeCount - 1);
	}

	// 初始化二维矩阵
	matrix.transet = new string* [matrix.nodeCount];
	for (i = 0; i < matrix.nodeCount; i++)
	{
		matrix.transet[i] = new string[matrix.nodeCount];
	}
	for (i = 0; i < matrix.nodeCount; i++)
	{
		for (j = 0; j < matrix.nodeCount; j++)
		{
			matrix.transet[i][j] = '$';	// '$'表示无数据
		}
	}
	matrix.StartNode = nfa_cell.StartNode;
	matrix.EndNodeSet = nfa_cell.EndNodeSet;
	matrix.transchar = nfa_cell.transchar;

	for (i = 0; i < nfa_cell.EdgeCount; i++)
	{
		int start, end;
		start = find_index_in_vertex(matrix, nfa_cell.EdgeSet[i].Start.Name);
		end = find_index_in_vertex(matrix, nfa_cell.EdgeSet[i].End.Name);
		if (start != -1 && end != -1)
		{
			if (matrix.transet[start][end] == "$")
			{
				matrix.transet[start][end] = "";
			}
			matrix.transet[start][end] += nfa_cell.EdgeSet[i].transchar;
			matrix.edgeCount++;
		}
		else
		{
			cerr << "error in transfer from cell to matrix!" << endl;
			return Matrix(matrix);
		}
	}

	return matrix;
}

// 找到节点在矩阵名称中的位置，返回下标
int find_index_in_vertex(Matrix matrix, char ch)
{
	int i;
	for (i = 0; i < matrix.nodeCount; i++)
	{
		if (matrix.vertex[i] == ch)
		{
			return i;
		}
	}
	return -1;
}

// 重载输出matrix;
ostream& operator<<(ostream& out, const Matrix& nfa)
{
	int i = 0, j, count = 0;
	out << "自动机M 的边数：" << nfa.edgeCount << endl;
	out << "自动机M 的起始状态：" << nfa.StartNode.Name << endl;
	out << "自动机M 的结束状态：";
	vector<node>::const_iterator node_iter;
	for (node_iter = nfa.EndNodeSet.begin(); node_iter != nfa.EndNodeSet.end(); node_iter++)
	{
		out << (*node_iter).Name << " ";
	}
	out << endl;
	out << "自动机M 的转换字符集有:" << endl;
	set<char>::iterator iter;
	for (iter = nfa.transchar.begin(); iter != nfa.transchar.end(); iter++)
	{
		out << *iter << " ";
	}
	out << endl << endl;

	out << " " << " ";
	for (i = 0; i < nfa.nodeCount; i++)
	{
		out << nfa.vertex[i] << " ";
	}
	out << endl;
	for (i = 0; i < nfa.nodeCount; i++)
	{
		out << nfa.vertex[i] << " ";
		for (j = 0; j < nfa.nodeCount; j++)
		{
			if (nfa.transet[i][j] == "$")
			{
				out << "-" << " ";
			}
			else
			{
				out << nfa.transet[i][j] << " ";
			}
		}
		out << endl;
	}

	out << "结束" << endl;

	return out;
}
#pragma endregion

#pragma region NFA-DFA

// 将矩阵形式转换为cell形式
cell matrix2cell(Matrix nfa)
{
	cell newCell;
	newCell.EdgeCount = 0;
	newCell.StartNode = nfa.StartNode;
	newCell.EndNodeSet = nfa.EndNodeSet;
	newCell.transchar = nfa.transchar;

	int i, j;
	for (i = 0; i < nfa.nodeCount; i++)
	{
		for (j = 0; j < nfa.nodeCount; j++)
		{
			if (nfa.transet[i][j] != "$")
			{
				for (int m = 0; m < nfa.transet[i][j].size(); m++)
				{
					edge newEdge;
					newEdge.Start.Name = nfa.vertex[i];
					newEdge.End.Name = nfa.vertex[j];
					newEdge.transchar = nfa.transet[i][j][m];

					newCell.EdgeSet[newCell.EdgeCount++] = newEdge;
				}
			}
		}
	}
	return newCell;
}

// 将NFA_matrix转换为DFA_matrix
Matrix NFA2DFA(Matrix nfa)
{
	Matrix dfa;
	vector<set<char>> Q;

	// 我们应在Q集合中元素数量不再变化时停止循环，运用一个队列结构来实现
	queue<set<char>> s;
	// 初始时将开始节点的ε闭包入队列
	s.push(e_closure(nfa,nfa.StartNode.Name));
	Q.push_back(e_closure(nfa, nfa.StartNode.Name));

	// 另外最好能保存转换结果，这里用一个队列来完成
	queue<set<char>> trans_edge;

	// 子集法获取新节点
	while (!s.empty())
	{
		set<char> top = s.front();		// 取出队首集合
		s.pop();

		// 对每一个转换字符，求转换结果
		set<char>::iterator transchar_iter;
		for (transchar_iter = nfa.transchar.begin(); transchar_iter != nfa.transchar.end(); transchar_iter++)
		{
			set<char> temp;
			int flag = 0;

			// 遍历所取出集合中的每一个元素，求其转换结果的ε-闭包，并合并到temp中
			set<char>::iterator top_iter;
			for (top_iter = top.begin(); top_iter != top.end(); top_iter++)
			{
				temp = union_set(temp, transfer_result(nfa, *top_iter, *transchar_iter));
			}

			if (temp.size() == 0)
			{
				temp.insert('$');
			}
			else {
				// 遍历Q，看是否已有与temp相同的集合，没有则加入栈中
				vector<set<char>>::iterator Q_iter;
				for (Q_iter = Q.begin(); Q_iter != Q.end(); Q_iter++)
				{
					if (equal_set(*Q_iter, temp))
					{
						flag = 1;
					}
				}
				if (flag == 0)
				{
					s.push(temp);
					Q.push_back(temp);
				}
			}
			trans_edge.push(temp);		// 将转换结果进队列
		}
	}

	#pragma region 利用新节点来构造DFA

	int i, j;
	// 初始化点数、边数、转换字符集
	dfa.nodeCount = Q.size();
	dfa.edgeCount = trans_edge.size();
	dfa.transchar = nfa.transchar;

	// 初始化节点名称
	dfa.vertex = new char[dfa.nodeCount];
	for (i = dfa.nodeCount - 1, j = 65; i >= 0; i--, j--)
	{
		dfa.vertex[i] = char(j + dfa.nodeCount - 1);
	}

	// 初始化二维矩阵
	dfa.transet = new string*[dfa.nodeCount];
	for (i = 0; i < dfa.nodeCount; i++)
	{
		dfa.transet[i] = new string[dfa.nodeCount];
	}
	for (i = 0; i < dfa.nodeCount; i++)
	{
		for (j = 0; j < dfa.nodeCount; j++)
		{
			dfa.transet[i][j] = '$';
		}
	}

	// 从队列中给矩阵赋值
	i = 0;
	while (!trans_edge.empty())
	{
		set<char>::iterator trans_iter;
		for (trans_iter = dfa.transchar.begin(); trans_iter != dfa.transchar.end(); trans_iter++)
		{
			set<char> temp = trans_edge.front();
			trans_edge.pop();
			if (temp.size() == 1 && *temp.begin() == '$')
			{
				// do nothing
			}
			else {
				for (j = 0; j < Q.size(); j++)
				{
					if (equal_set(temp, Q[j]))
					{
						break;
					}
				}
				if (dfa.transet[i][j] == "$")
				{
					dfa.transet[i][j] = "";
				}
				dfa.transet[i][j] += *trans_iter;
			}
		}
		i++;
	}

	// 开始节点为第一个节点
	node start;
	start.Name = dfa.vertex[0];
	dfa.StartNode = start;

	// 构造终止节点集
	for (i = 0; i < Q.size(); i++)
	{
		set<char>::iterator temp_iter;
		temp_iter = Q[i].find(nfa.EndNodeSet[0].Name);
		if (temp_iter != Q[i].end())
		{
			node end;
			end.Name = dfa.vertex[i];
			dfa.EndNodeSet.push_back(end);
		}
	}
	#pragma endregion

	return dfa;
}
	
// 返回一个节点经过转换字符转换得到的结果（ε闭包结果）
set<char> transfer_result(Matrix nfa, char start, char transchar)
{
	set<char> result;
	int index = find_index_in_vertex(nfa, start);
	for (int i = 0; i < nfa.nodeCount; i++)
	{
		if (nfa.transet[index][i].find(transchar)!=string::npos)
		{
			// 将其ε-闭包加入结果
			result = union_set(result, e_closure(nfa, nfa.vertex[i]));
		}
	}
	return result;
}

// 返回一个节点的ε闭包
set<char> e_closure(Matrix nfa, char start_node)
{
	int index;
	set<char> result;
	
	stack<char> s;
	s.push(start_node);

	while (!s.empty())
	{
		char ch = s.top();
		s.pop();
		result.insert(ch);
		index = find_index_in_vertex(nfa, ch);
		for (int i = 0; i < nfa.nodeCount; i++)
		{
			if (nfa.transet[index][i] == "#")
			{
				s.push(nfa.vertex[i]);
			}
		}
	}

	return result;
}

// 将两个集合合并
set<char> union_set(set<char>s1, set<char>s2)
{
	s1.insert(s2.begin(), s2.end());
	return s1;
}

// 判断两个集合是否相同
bool equal_set(set<char> s1, set<char> s2)
{
	// 大小不一样的集合必然不同
	if (s1.size() != s2.size())
	{
		return false;
	}

	set<char> temp;
	temp = union_set(s1, s2);
	
	if (temp.size() != s1.size())
	{
		return false;
	}
	else
	{
		return true;
	}
}

#pragma endregion

#pragma region DFA最小化

Matrix DFA_minimize(Matrix dfa)
{
	vector<set<char>> groups;

	// 初始化groups,分成终止集和非终止集
	groups = init_groups(dfa);

	// 循环进行分组
	groups = grouping(dfa, groups);

	// groups中获得了新节点数，先初始化dfa_min的节点信息
	int i, j;
	Matrix dfa_min;
	dfa_min.nodeCount = groups.size();
	dfa_min.edgeCount = 0;
	dfa_min.transchar = dfa.transchar;
	dfa_min.vertex = new char[dfa_min.nodeCount];
	for (i = dfa_min.nodeCount - 1, j = 65; i >= 0; i--, j--)
	{
		dfa_min.vertex[i] = char(j + dfa_min.nodeCount - 1);
	}

	vector<node>::iterator end_iter;
	for (i = 0; i < groups.size(); i++)
	{
		if (groups[i].find(dfa.StartNode.Name) != groups[i].end())
		{
			dfa_min.StartNode.Name = dfa_min.vertex[i];
		}
		for (end_iter = dfa.EndNodeSet.begin(); end_iter != dfa.EndNodeSet.end(); end_iter++)
		{
			if (groups[i].find((*end_iter).Name) != groups[i].end())
			{
				node newEnd;
				newEnd.Name = dfa_min.vertex[i];
				dfa_min.EndNodeSet.push_back(newEnd);
				break;
			}
		}
	}

	// 初始化边集
	dfa_min.transet = new string * [dfa_min.nodeCount];
	for (i = 0; i < dfa_min.nodeCount; i++)
	{
		dfa_min.transet[i] = new string[dfa_min.nodeCount];
	}
	for (i = 0; i < dfa_min.nodeCount; i++)
	{
		for (j = 0; j < dfa_min.nodeCount; j++)
		{
			dfa_min.transet[i][j] = "$";
		}
	}

	// 求新的边集
	set<char>::iterator trans_iter;
	for (i = 0; i < groups.size(); i++)
	{
		for (trans_iter = dfa.transchar.begin(); trans_iter != dfa.transchar.end(); trans_iter++)
		{
			set<char> trans_result;
			set<char>::iterator set_iter;
			for (set_iter = groups[i].begin(); set_iter != groups[i].end(); set_iter++)
			{
				trans_result = union_set(trans_result,(transfer_result(dfa, *set_iter, *trans_iter)));
			}
			if (trans_result.size() != 0)
			{
				for (j = 0; j < groups.size(); j++)
				{
					if (sub_set(groups[j], trans_result))
					{
						break;
					}
				}
				if (dfa_min.transet[i][j] == "$")
				{
					dfa_min.transet[i][j] = "";
				}
				dfa_min.transet[i][j] += *trans_iter;
				dfa_min.edgeCount++;
			}
		}
	}

	return dfa_min;
}

// 初始化一个将节点分为“终结集”和“非终结集”的容器
vector<set<char>> init_groups(Matrix dfa)
{
	vector<set<char>> groups;
	
	// 首先划分出终结集
	set<char> endgroup;
	vector<node>::iterator endnode_iter;
	for (endnode_iter = dfa.EndNodeSet.begin(); endnode_iter != dfa.EndNodeSet.end(); endnode_iter++)
	{
		endgroup.insert((*endnode_iter).Name);
	}
	groups.push_back(endgroup);

	// 然后划分出非终结集
	set<char> not_endgroup;
	int i;
	for (i = 0; i < dfa.nodeCount; i++)
	{
		if (endgroup.find(dfa.vertex[i]) == endgroup.end())
		{
			not_endgroup.insert(dfa.vertex[i]);
		}
	}
	if (not_endgroup.size() != 0)		// 有可能所有节点都是终止结点，所以仅当非终结集不为空时加入
	{
		groups.push_back(not_endgroup);
	}

	return groups;
}

// 从“终结集”和“非终结集”开始，进行分组
vector<set<char>> grouping(Matrix dfa, vector<set<char>> groups)
{
	// 如果所有节点都是终止结点那就不需要做分组，否则进行循环分组 
	int i, j, k, m;
	if (groups.size() != 1)
	{
		int c = groups.size(), p = 0;	// 计数器，当groups中不再加入新组时(c==p)循环结束
		while (c != p)
		{
			p = c;
			int flag = 0;	// 分组标志，0表示不需要分组，1表示需要

			// 对groups中每个集合，检查是否需要分组
			for (i = 0; i < groups.size(); i++)
			{
				// 对每个转换字符，求转换结果，检查是否需要分组
				set<char>::iterator transchar_iter;
				for (transchar_iter = dfa.transchar.begin(); transchar_iter != dfa.transchar.end(); transchar_iter++)
				{
					// 求转换结果
					vector<char> trans_result;
					set<char>::iterator set_iter;
					for (set_iter = groups[i].begin(); set_iter != groups[i].end(); set_iter++)
					{
						// 由于dfa中节点经过一个转换字符只能到一个终点，因此直接取begin()即可
						// 不过还要考虑空集的问题
						set<char> temp = transfer_result(dfa, *set_iter, *transchar_iter);
						if (temp.size() == 0)
						{
							temp.insert('$');
						}
						trans_result.push_back(*(temp.begin()));
					}

					// 检查是否需要分组
					vector<int> grouping;
					for (j = 0; j < trans_result.size(); j++)
					{
						if (trans_result[j] == '$')
						{
							grouping.push_back(-1);
						}
						else
						{
							for (k = 0; k < groups.size(); k++)
							{
								if (groups[k].find(trans_result[j]) != groups[k].end())
								{
									grouping.push_back(k);
									break;
								}
							}
						}
					}

					// 如果来自不止一个组，则需要分组
					set<int> group_num(grouping.begin(), grouping.end());
					if (group_num.size() != 1)
					{
						flag = 1;

						set<int>::iterator grouping_iter;
						for (grouping_iter = group_num.begin(); grouping_iter != group_num.end(); grouping_iter++)
						{
							set<char> new_group;
							set<char>::iterator set_iter;
							for (set_iter = groups[i].begin(), m = 0; set_iter != groups[i].end(); set_iter++, m++)
							{
								if (grouping[m] == *grouping_iter)
								{
									new_group.insert(*set_iter);
								}
							}
							groups.push_back(new_group);
						}
						
						groups.erase(groups.begin() + i);
						break;
					}
				}

				if (flag == 1)
				{
					break;
				}
			}
			c = groups.size();
		}
	}
	return groups;
}

// 判断s2是否是s1子集
bool sub_set(set<char> s1, set<char> s2)
{
	if (s2.size() > s1.size())
		return false;
	
	set<char>::iterator set_iter;
	for (set_iter = s2.begin(); set_iter != s2.end(); set_iter++)
	{
		if (s1.find(*set_iter) == s1.end())
		{
			return false;
		}
	}
	return true;
}
#pragma endregion
