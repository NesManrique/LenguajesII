%{
#include "ast.h"
#include "parser.hpp"
#include <math.h>
#include <string>
#define TOKEN(t) (yylval.token = t)
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define SAVE_STRING yylval.string = new std::string(string_buf)
#define MAX_STR_CONST 1024
int yycolumn = 1;
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;\
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn+yyleng-1;\
    yycolumn +=yyleng;

char string_buf[MAX_STR_CONST];
char *string_buf_ptr;
int string_len;
extern "C" int yywrap() { }

%}
     
DIGIT    [0-9]
ID       [a-zA-Z][a-zA-Z0-9_]*
DOUBLE	 {DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?

%x COMMENT
%x STRING
%x STRING2
%x COMMENTL
%x CHARC

%%

<INITIAL>{
    "/*"				BEGIN(COMMENT);
	\"					{string_buf_ptr=string_buf;BEGIN(STRING);}
	\'					{string_buf_ptr=string_buf;BEGIN(STRING2);}
	"//"				BEGIN(COMMENTL);
    "#"                 BEGIN(CHARC);

	{DIGIT}+			{if((yylval.integer=strtol(yytext,NULL,0))<=-2147483648){
                            fprintf(stderr,"Warning: Underflow detected in line %d\n", yylineno);
                         }else if((yylval.integer=strtol(yytext,NULL,0))>=2147483647){
                                fprintf(stderr,"Warning: Overflow detected in line %d\n",yylineno);}
                        return INT;}

	{DOUBLE}			{   errno=0;
                            if((yylval.floating=strtod(yytext,NULL))==HUGE_VAL){
                                perror("Warning floating point number cause overflow\n");
                            }else if((yylval.floating=strtod(yytext,NULL))==0 && 
                             errno==ERANGE){
                                perror("Warning floating point number cause underflow\n");
                            }
                         return FLOAT;}

	[(){}\[\],.!=<>%]		{return yytext[0];}

	"is"				{return '=';}

	"=="				{return EQ;}
	"<="				{return LEQ;}
	">="				{return GEQ;}
	"!="				{return NEQ;}
	"->"				{return ACCESS;}

	"&&"				|
	"and"				{return AND;}
	"||"				|
	"or"				{return OR;}
	"^"					|
	"if"				{return IF;}
	"then"				{return THEN;}
	"else"				{return ELSE;}
	"true"				{return TRUE;}
	"false"				{return FALSE;}

	"while"				{return WHILE;}
	"do"				{return DO;}
	"stop"				{return STOP;}
	"next"				{return NEXT;}
	
	"for"				{return FOR;}
	"from"				{return FROM;}
	"in"				{return IN;}
	"to"				{return TO;}
	"return"			{return RETURN;}
	"step"				{return STEP;}
	

	"+"					|
	"plus"				{return '+';}
	"-"					|
	"minus"				{return '-';}
	"/"					|
	"dividedby"			{return '/';}
	"*"					|
	"times"				{return '*';}

	"not"				{return '!';}

	/*
	"integer"			{return INT;}
	"float"				{return FLOAT;}
	"character"			{return CHAR;}
	"boolean"			{return BOOL;}
	"void"				{return VOID;}
*/	
	"string"			{return STRIN;}
	"register"			{return REGISTER;}
	"union"				{return UNION;}
	"arrayof"			{return ARRAY;}

	/*"function"			{return FUN;}*/

	{ID}				{SAVE_TOKEN; return ID;}

	\n				    {yylineno++;
                            yycolumn=1;}
	[ \t]                /* Ignore whitespace */
}

<COMMENT>{
   "*/"      BEGIN(INITIAL);
    [^*\n]+   // eat comment in chunks
	"*"       // eat the lone star
	\n        yylineno++;
}

<COMMENTL>{
	[^\n]+	
	\n		{yylineno++;BEGIN(INITIAL);}
}


<STRING>{
	\"					{ /* saw closing quote - all done */
							BEGIN(INITIAL);
							*string_buf_ptr = '\0';
							/* return string constant token type and
							* value to parser
							*/
							SAVE_STRING;
                            cout << "yylval " << *yylval.string <<endl;
                            printf("str buf %s\n",string_buf);
							return STR;
						}
				
	\n					{/* error - unterminated string constant */
						/* generate error message */
                             printf("Unterminated string\n");
                             yycolumn=1;
						}

						
	\\n					*string_buf_ptr++ = '\n';
	\\v					*string_buf_ptr++ = '\v';
	\\t					*string_buf_ptr++ = '\t';
	\\r					*string_buf_ptr++ = '\r';
	\\b					*string_buf_ptr++ = '\b';
	\\f					*string_buf_ptr++ = '\f';
	\\"					*string_buf_ptr++ = '"';

	[^\\\n\"]+			{char *yptr = yytext;
						while ( *yptr )
							*string_buf_ptr++ = *yptr++;
						}
}	 

<STRING2>{
	\'					{ /* saw closing quote - all done */
							BEGIN(INITIAL);
							*string_buf_ptr = '\0';
							/* return string constant token type and
							* value to parser
							*/
							SAVE_STRING;
                            cout << "yylval " << yylval.string <<endl;
							return STR;
							
						}
				
	\n					{/* error - unterminated string constant */
						/* generate error message */
                             printf("Unterminated string\n");
                            yycolumn = 1;
						}

						

	[^\']+			{char *yptr = yytext;
						while ( *yptr )
							*string_buf_ptr++ = *yptr++;
						}
}

<CHARC>{

    \n                  {printf("Character Error\n");
                            yycolumn = 1;}

    \\n					{yylval.Char = '\n';
                            BEGIN(INITIAL);
                            return CHAR;}
	\\v					{yylval.Char = '\v';
                            BEGIN(INITIAL);
                            return CHAR;}
	\\t					{yylval.Char = '\t';
                            BEGIN(INITIAL);
                            return CHAR;}
	\\r					{yylval.Char = '\r';
                            BEGIN(INITIAL);
                            return CHAR;}
	\\b					{yylval.Char = '\b';
                            BEGIN(INITIAL);
                            return CHAR;}
	\\f					{yylval.Char = '\f';
                            BEGIN(INITIAL);
                            return CHAR;}
    \\s                 {yylval.Char = ' ';
                            BEGIN(INITIAL);
                            return CHAR;}

    .                   {yylval.Char = yytext[0];
                            BEGIN(INITIAL);
                            return CHAR;}
}
 
%%
     
