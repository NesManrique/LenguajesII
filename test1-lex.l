%{
#include <math.h>
#include "y.tab.h"

%}
     
DIGIT    [0-9]
ID       [a-z][a-z0-9]*
     
%%
  
[ \t]                  /* Ignore whitespace */
{DIGIT}+("."{DIGIT}*)? { sscanf(yytext,"%lf",&yylval); return NUM; }
"+"|"-"|"*"|"/"|"^"|\n return yytext[0];
     
%%
     
yywrap() { }
