#include <iostream>
#include <cstdio>
#include <vector>
#include "ast.h"
#include "symtable.h"

extern int yyparse();
using namespace std;
#ifdef DEBUG
extern int yydebug;
#endif

extern int yyparse();
extern NBlock *ProgramAST;
extern Symtable Table;

int main(int argc, char **argv)
{
#ifdef DEBUG	
		yydebug=1;
#endif
		freopen(argv[1],"r",stdin);
	    if(yyparse()==0)
            cout << "Todo bien" << endl;
		cout<<ProgramAST<<" "<<&Table<<" ";
		cout<<ProgramAST->typeChk()<<endl;

	    return 0;
}
