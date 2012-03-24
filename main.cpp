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
extern int flagerror;


int main(int argc, char **argv)
{
#ifdef DEBUG	
		yydebug=1;
#endif
		freopen(argv[1],"r",stdin);

		yyparse();
	    if(!flagerror){
            cout << "Parsing is over" << endl;
#ifdef DEBUG	
			ProgramAST->printTree(cout);
#endif
     	  	cout << "Beginning Type Check" << endl;
			Table.resetScope();
			cout << ProgramAST->typeChk(Table)<< endl;
 	      	cout << "Type Check Finished" << endl;
		}else{
            cout << "flagerror" <<endl;
        }
	    return 0;
}
