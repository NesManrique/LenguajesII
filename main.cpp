#include <iostream>
#include <cstdio>
#include "ast.h"
extern int yyparse();


int main(int argc, char **argv)
{
		freopen(argv[1],"r",stdin);
	    yyparse();
		
	    return 0;
}
