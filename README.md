# 词法分析程序

*Nie Zili*
*Last Update*
*10.4*

## 一、正规式——NFA

### 1.1 思路

节点构造采用[Thompson算法](https://zhuanlan.zhihu.com/p/54291684)，主要的难点在于如何让计算机正确识别输入的正规式。这里采用将中缀表达式转换为后缀表达式的方法，例如：

> $a(a|b)^*((ab)|c)^* → aab|^*+ab+c|^*+$

其中的'+'是为了方便修改为后缀而添加的分隔符。这样转换之后，我们只需要利用一个栈结构就可以获得正确的表达式顺序了（遇到运算符pop所需个数的运算数）。当然中缀转后缀需要用到双栈法（类似结构即可，数组也可以）。

在此部分我们使用一个比较具象的结构体来表示自动机，具体定义如下：

```c++
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
	char transchar;		        // 转换函数 Start-transchar-End
};

// NFA与DFA单元定义
// 该结构体是NFA整体的定义，但同时也用于构造过程中的临时结点构成
struct cell
{
	edge EdgeSet[MAX];		    // 边集
	int EdgeCount;			    // 边数
	set<char> transchar;	    // 转换函数名称集 (a,b,c...)
	node StartNode;			    // 开始节点，默认只有一个开始结点
	vector<node> EndNodeSet;	// 终止节点集，对于Thompson算法生成的NFA只有一个终止结点，但是DFA会有多个
};
```

### 1.2 输入正规式

要求用户输入正规式，需要保证其输入内容在给定的范围内('a'-'z'|'A'-'Z')，另一方面，需要确保用户输入的正规式中括号是匹配的。

### 1.3 中缀转后缀

中缀表达式如果直接转为后缀表达式，会因为没有括号而产生歧义，例如:

> $a(a|b)^*((ab)|c)^* → aab|^*abc|^*$

则原本的$((ab)|c)^*$会被解释为$a(b|c)^*$。因此在做中缀转后缀之前我们需要先将中缀表达式做适当的分隔，其具体规则如下：

1. 若第二个是字母、第一个不是'('、'|'要添加分隔符
2. 若第二个是'(',第一个不是'|'、'(',也要加分隔符

举例如下：

> $(ab)^*(a|c)^*→(a+b)^*+(a|c)^*$

添加分隔符之后就可以按照双栈法将其转变为后缀表达式了，其中用到的运算符优先级表如下：

| c1\c2 | '('  | '*'  | '\|' | '+'  | ')'  | '#'  |
| :---- | :--- | :--- | :--- | :--- | :--- | :--- |
| '('   | <    | <    | <    | <    | =    | >    |
| '*'   | <    | >    | >    | >    | >    | >    |
| '\|'  | <    | <    | >    | >    | >    | >    |
| ')'   | =    | >    | >    | >    | >    | >    |
| '+'   | <    | <    | <    | >    | >    | >    |
| '#'   | <    | <    | <    | <    | <    | =    |

由该表生成isp和icp用到的算法可以参考[博客](https://blog.csdn.net/qq_43432875/article/details/102515831)

### 1.4 生成NFA

在获得后缀表达式之后，对其进行遍历，并作如下操作：

1. 如遇到字符('a'-'z'|'A'-'Z')，则按照Thompson算法初始化一个NFA节点，将节点压入栈中。其次，检查转换字符集中是否有当前字符，若没有则加入
2. 如遇到运算符，则从栈中弹出其所需的运算数，并按照Thompson算法进行修改，将修改结果压入栈中
3. 重复1、2直到遍历结束

## 二、NFA-DFA

### 2.1 思路

NFA-DFA主要使用子集法，不过我们按照Thompson算法构造的NFA需要使用ε-closure的子集法进行，完整的算法可以参考[博客](https://blog.csdn.net/qq_37236745/article/details/83540177)

从这里开始使用矩阵形式表示的自动机会比较方便进行，其定义如下：

```c++
// NFA与DFA的矩阵结构定义
struct Matrix
{
	char* vertex;			// 节点名称，其大小为vertex[nodeCount]
	set<char> transchar;	// 转换函数名称集 (a,b,c...)
	char** transet;			// 转换矩阵，其大小为transet[nodeCount][nodeCount]
	int edgeCount;			// 边数统计
	int nodeCount;			// 节点统计
	node StartNode;			// 开始节点
	vector<node> EndNodeSet;	// 终止节点集
};
```

使用矩阵表示的一大好处是可以快速的找到一个节点的出度和入度，这对于求取ε闭包以及后面DFA确定化划分集合有所帮助。`cell`与`Matrix`可以互相转换，转换函数我已实现。

### 2.2 ε-closure

求取ε闭包的过程使用图的深度遍历方法即可，详见代码

### 2.3 子集法

从开始节点的ε闭包开始，对每一个转换字符求取转换结果，将新的转换节点加入新增节点集合Q中，并对新增节点重复上述操作。直到不再产生新的节点，则新增节点集合Q构造完毕，Q即为DFA的节点集合。

以上是子集法的大致描述，为了实现上面的算法，我们需要一些数据结构来存储相应的内容：

1. 新增节点集合Q：`vector<set<char>> Q`
2. 不再产生新节点:`queue<set<char>> s`
   
另外一方面，我希望在求取Q的同时可以获得转换矩阵，因此"对每一个转换字符求取转换结果"这一步操作的结果最好能够保存下来，并且还要求能够和Q中节点做到一一对应。这里我用一个队列来完成：`queue<set<char>> trans_edge`

伪代码描述如下：

```c++
void get_Q(Matrix nfa)
{
    vector<set<char>> Q;            // 新增节点集合
    queue<set<char>> s;             // 计算队列
    queue<set<char>> trans_edge;    // 新节点的转换终点集

    Q.insert(e-closure(nfa.StartNode));     // 初始状态将开始节点的ε闭包加入Q与s
    s.push(e-closure(nfa.StartNode));
    
    while(!s.empty())
    {
        set<char> top = s.front();  // 取出计算栈的队首元素
        s.pop();

        // 对每个转换字符，求转换结果
        for(tran in nfa.transchar)
        {
            set<char> tran_result;
            tran_result = transfer(top,tran);

            // 如果当前节点经过转换字符转换到空集，则加入一个表示空集的符号
            if(tran_result.size()==0)
            {
                tran_result.insert('$');
            }
            else
            {
                // 如果当前转换结果不在Q中，则加入Q，并加入到计算队列s
                if(!find(Q,tran_result))
                {
                    s.push(tran_result);
                    Q.push_back(tran_result);
                }
            }
            // 将当前转换结果加入队列
            trans_edge.push(tran_result);
        }
    }
}
```

举个例子，如(a|b)*，运行第一部分生成NFA后获得一个以G为起点，H为终点的自动机，其ε-closure子集法的表格如下：

|  | 'a'  | 'b'  |
| :---- | :--- | :--- |
| {GEACH}   | {BFEACH}    | {DFEACH}    |
| {BFEACH}   | {BFEACH}    | {DFEACH}     |
| {DFEACH}   | {BFEACH}    | {DFEACH}     |

构造该表过程中，Q、s、trans_edge的变化如图：