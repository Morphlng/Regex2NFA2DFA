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

	Matrix nfa;
	nfa = cell2matrix(NFA_cell);
	// cout << nfa << endl << endl;
	#pragma endregion
	
	#pragma region NFA转DFA
	
	Matrix dfa;
	dfa = NFA2DFA(nfa);
	// cout << dfa;
	
	#pragma endregion

	#pragma region DFA最小化

	Matrix dfa_m;
	dfa_m = DFA_minimize(dfa);
	cout << dfa_m << endl;
	#pragma endregion


	return 0;
}