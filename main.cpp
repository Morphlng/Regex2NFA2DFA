#include"Regex2NFA2DFA.h"

int main()
{
	#pragma region 正规式生成NFA
	string regex = input_regex();
	cout << "regex:" << regex << endl;
	string separated_regex = add_separation(regex);
	cout << "separated_regex:" << separated_regex << endl;
	string postfix_regex = to_postfix(separated_regex);
	cout << "postfix_regex:" << postfix_regex << endl;

	cell NFA_cell;
	NFA_cell = regex_to_nfa(postfix_regex);
	// cout << NFA_cell;

	Matrix nfa_m;
	nfa_m = cell2matrix(NFA_cell);
	// cout << nfa_m << endl << endl;
	#pragma endregion
	
	#pragma region NFA转DFA
	
	Matrix dfa_m;
	dfa_m = NFA2DFA(nfa_m);
	cout << dfa_m;
	
	#pragma endregion

	#pragma region DFA最小化



	#pragma endregion


	return 0;
}