%{
#include "y.tab.h"
#include <math.h>
#define TOKEN(t) (yylval.token = t)
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
char string_buf[MAX_STR_CONST];
char *string_buf_ptr;

%}
     
DIGIT    [0-9]
ID       [a-zA-Z][a-zA-Z0-9_]*
DOUBLE	 {DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?

%x COMMENT
%x STRING
%x STRING2

%%

<INITIAL>{
    "/*"				BEGIN(COMMENT);
	\"					BEGIN(STRING);
	\'					BEGIN(STRING2);
	"//[^\n]"			yylineno++;
	{DIGIT}+			{yylval=atoi(yytext);return INT}
	{DOUBLE}			{sscanf(yytext,"%lf",&yylval);return FLOAT}

	[(){},.!=<>%]		{return yytext[0];}

	"is"				{return '=';}

	"=="				{return EQUALS;}
	"<="				{return LEQUALS;}
	">="				{return GEQUALS;}
	"!="				{return NOTEQUALS;}

	"&&"				|
	"and"				{return AND;}
	"||"				|
	"or"				{return OR;}
	"^"					|
	"xor"				{return XOR;}
	"if"				{return IF;}
	"then"				{return THEN;}
	"else"				{return ELSE;}
	"true"				{return TRUE;}
	"false"				{return FALSE;}

	"while"				{return FOR;}
	"do"				{return DO;}
	"stop"				{return STOP;}
	"next"				{return NEXT;}
	
	"for"				{return FOR;}
	"from"				{return FROM;}
	"to"				{return TO;}
	"return"			{return RETURN;}
	

	"+"					|
	"plus"				{return '+';}
	"-"					|
	"minus"				{return '-';}
	"/"					|
	"dividedby"			{return '/';}
	"*"					|
	"times"				{return '*';}

	"not"				{return '!';}



	"integer"			{return INT;}
	"float"				{return FLOAT;}
	"string"			{return STRING;}
	"character"			{return CHAR;}
	"boolean"			{return BOOL;}
	"void"				{return VOID;}
	
	"register"			{return REG;}
	"union"				{return UNION;}

	"function"			{return FUNC;}

	{ID}				{SAVE_TOKEN; return ID;}

	\n				yylineno++;
	[ \t]                  /* Ignore whitespace */
}

<COMMENT>{
   "*/"      BEGIN(INITIAL);
    [^*\n]+   // eat comment in chunks
	"*"       // eat the lone star
	\n        yylineno++;
}



<STRING>{
	\"					{ /* saw closing quote - all done */
							BEGIN(INITIAL);
							*string_buf_ptr = '\0';
							/* return string constant token type and
							* value to parser
							*/
						}
				
	\n					{/* error - unterminated string constant */
						/* generate error message */
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
	\\"					*string_buf_ptr++ = '"';

<STRING2>{
	\'					{ /* saw closing quote - all done */
							BEGIN(INITIAL);
							*string_buf_ptr = '\0';
							/* return string constant token type and
							* value to parser
							*/
						}
				
	\n					{/* error - unterminated string constant */
						/* generate error message */
						}

						

	[^\']+			{char *yptr = yytext;
						while ( *yptr )
							*string_buf_ptr++ = *yptr++;
						}
}	 
%%
     
yywrap() { }
