%{
#include "y.tab.h"
#include <math.h>
#define TOKEN(t) (yylval.token = t)
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
%}
     
DIGIT    [0-9]
ID       [a-z][a-z0-9]*

%x COMMENT
%x STRING

%%

<INITIAL>{
    "/*"				BEGIN(COMMENT);
	\"					BEGIN(STRING);
	"//[^\n]"			yylineno++;
	{DIGIT}+			{yylval=atoi(yytext);return INT}
	{DIGIT}.{DIGIT}+	{sscanf(yytext,"%lf",&yylval);return FLOAT}

	[(){},.!=<>%]		{return yytext[0];}

	"=="				{return EQUALS;}
	"<="				{return LEQUALS;}
	">="				{return GEQUALS;}
	"!="				{return NOTEQUALS;}

	"and"				{return AND;}
	"or"				{return OR;}
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

	"integer"			{return TYPEINT;}
	"float"				{return TYPEFLOAT;}
	"string"			{return TYPESTRING;}
	"character"			{return TYPECHAR;}
	"boolean"			{return TYPEBOOL;}
	
	"register"			{return TYPEREG;}

	"function"			{return DECFUNC;}

	

	\n				yylineno++;
	[ \t]                  /* Ignore whitespace */
}

<COMMENT>{
   "*/"      BEGIN(INITIAL);
    [^*\n]+   // eat comment in chunks
	"*"       // eat the lone star
	\n        yylineno++;
}

		char string_buf[MAX_STR_CONST];
		char *string_buf_ptr;

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
				
	\\[0-7]{1,3}		{/* octal escape sequence */
							int result;
							(void) sscanf( yytext + 1, "%o", &result );
							if ( result > 0xff )
								/* error, constant is out-of-bounds */
								*string_buf_ptr++ = result;
						}
						
	\\[0-9]+			{/* generate error - bad escape sequence; something
						* like '\48' or '\0777777'
						*/
						}
						
	\\n					*string_buf_ptr++ = '\n';
	\\t					*string_buf_ptr++ = '\t';
	\\r					*string_buf_ptr++ = '\r';
	\\b					*string_buf_ptr++ = '\b';
	\\f					*string_buf_ptr++ = '\f';
	
	\\(.|\n)			*string_buf_ptr++ = yytext[1];
	
	[^\\\n\"]+			{char *yptr = yytext;
						while ( *yptr )
							*string_buf_ptr++ = *yptr++;
						}
	
}	 
%%
     
yywrap() { }
