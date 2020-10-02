#include"Regex2NFA.h"

int main()
{
	string regex = input_regex();
	cout << "regex:" << regex << endl;
	string separated_regex = add_separation(regex);
	cout << "separated_regex:" << separated_regex << endl;
	string postfix_regex = to_postfix(separated_regex);
	cout << "postfix_regex:" << postfix_regex << endl;

	cell NFA_cell;
	NFA_cell = regex_to_nfa(postfix_regex);
	//cout << NFA_cell;

	NFA_Matrix m;
	m = cell2matrix(NFA_cell);
	cout << m;
	return 0;
}