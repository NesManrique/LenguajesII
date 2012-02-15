#include <iostream>
#include <cstdio>
#include "ast.h"

extern int yydebug;
extern int yyparse();
using namespace std;


int main(int argc, char **argv)
{
        yydebug=1;
		freopen(argv[1],"r",stdin);
	    if(yyparse()==0)
            cout << "Todo bien" << endl;
        //yyparse();
		
	    return 0;
}
