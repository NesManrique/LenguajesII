#include <iostream>
#include <cstdio>
#include <vector>
#include "ast.h"
#include "symtable.h"
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
	    yyparse();\
		cout<<ProgramAST<<" "<<&Table<<" ";
		cout<<ProgramAST->typeChk(Table)<<endl;
	    return 0;
}
