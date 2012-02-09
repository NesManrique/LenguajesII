%{
#include "ast.h"
#include "parser.hpp"
#include <math.h>
#include <string>
#define TOKEN(t) (yylval.token = t)
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define SAVE_STRING yylval.string = new std::string(string_buf,string_len)
#define MAX_STR_CONST 1024

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

%%

<INITIAL>{
    "/*"				BEGIN(COMMENT);
	\"					{string_buf_ptr=string_buf;BEGIN(STRING);}
	\'					{string_buf_ptr=string_buf;BEGIN(STRING2);}
	"//"				BEGIN(COMMENTL);
	{DIGIT}+			{yylval.integer=atoi(yytext);return INT;}
	{DOUBLE}			{sscanf(yytext,"%lf",&yylval.floating);return FLOAT;}

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
	"string"			{return STRING;}
	"character"			{return CHAR;}
	"boolean"			{return BOOL;}
	"void"				{return VOID;}
*/	
	"register"			{return REGISTER;}
	"union"				{return UNION;}

	"function"			{return FUN;}

	{ID}				{SAVE_TOKEN; return ID;}

	\n				yylineno++;
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
							return STR;
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

<STRING2>{
	\'					{ /* saw closing quote - all done */
							BEGIN(INITIAL);
							*string_buf_ptr = '\0';
							/* return string constant token type and
							* value to parser
							*/
							SAVE_STRING;
							return STR;
							
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
     
