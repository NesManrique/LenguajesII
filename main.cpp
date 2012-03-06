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
        /*Table.lookupType("character");
        Table.lookupType("integer");
        Table.lookupType("float");
        Table.lookupType("boolean");
        Table.insert("sdasd",new TVar());
        Table.insert("qweqwr",new TVar());
        Table.insert("zxczxc",new TVar());
        Table.insert("fghhf",new TVar());*/
	    if(yyparse()==0)
            cout << "Parsing is over" << endl;
        cout << "Beginning Type Check" << endl;
		cout << ProgramAST->typeChk(Table)->name;
        cout << "Type Check Finished" << endl;
	    return 0;
}
