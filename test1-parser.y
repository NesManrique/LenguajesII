%{

#define YYSTYPE double
#include <math.h>
#include <stdio.h>

int yylex (void);
void yyerror (char const *);

%}
     
/* Bison declarations.  */
%token NUM
%left '-' '+'
%left '*' '/'
%left NEG     /* negation--unary minus */
%right '^'    /* exponentiation */
     
%% /* The grammar follows.  */
input :    /* empty */
      | input line
      ;
     
line  :     '\n'
      | exp '\n'  { printf ("\t%.10g\n", $1); }
      ;
     
exp   : NUM                { $$ = $1;         }
      | exp '+' exp        { $$ = $1 + $3;    }
      | exp '-' exp        { $$ = $1 - $3;    }
      | exp '*' exp        { $$ = $1 * $3;    }
      | exp '/' exp        { $$ = $1 / $3;    }
      | '-' exp  %prec NEG { $$ = -$2;        }
      | exp '^' exp        { $$ = pow ($1, $3); }
      | '(' exp ')'        { $$ = $2;         }
      ;
%%

void yyerror (char const *s)
{
  fprintf (stderr, "%s\n", s);
}

void main (int argc,char **argv)
{
  yyparse ();
}
