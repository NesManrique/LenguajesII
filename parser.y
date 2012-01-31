%{
#include "node.h"
#include <cstdio>
#include <string>
extern int yylex (void);
void yyerror (char const *a){printf("ERROR: %s\n",a);};

%}

/* Ways to access data */

%union{
	Node *node;
	NBlock	*expr;
	NStatement *stmt;
	NIdentifier	*ident;
	NVariableDeclaration *var_decl;
	NArrayDeclaration *arr_decl;
	NRegisterDeclaration *reg_decl;
	NUnionDeclaration *union_decl;
	std::vector<NVariableDeclaration*> *varvec;
	std::vector<NExpression*> *exprvec;
	std::string *string;
	int token;
	int integer;
	double floating;

}

/* Terminal Symbols */

%token  <string> STR
%token	<integer> INT
%token	<floating> FLOAT
/*%token	<token> '=' '(' ')' '{' '}' ',' '.' '!' '<' '>' '%'
%token	<token> '+' '-' '/' '*'*/
%token 	<token> IF THEN ELSE FROM TO IN NEXT STOP
%token	<token>	CHAR BOOL VOID REG UNION FUN TRUE FALSE
%token 	<token> REGISTER DO WHILE RETURN IS FOR 
%token	<string> ID

/* Type of node our nonterminal represent */
%type	<expr>	expr 
%type	<varvec> fun_decl_args 
%type	<exprvec>	fun_call_args /*array_elem*/
%type	<block>	program stmts block decl
%type	<stmt>	stmt var_decl fun_decl reg_decl 
%type	<stmt>	union_decl ctrl_while ctrl_if
/*%type	<token>	comparison*/

/* Matematical operators precedence */
%nonassoc	EQ NEQ GEQ LEQ '<' '>'	
%left	'+' '-' AND OR
%left	'*' '/'
%left 	NEG NOT
%left	ACCESS

%start program
%%
program 	: decls {}
			;

decls		: decl	{}
			| decls decl {}
			;

decl		: var_decl {}
			| reg_decl {}
			| union_decl {}
			| fun_decl {}
			;

var_decl	: ID ID {}
			| ID ID IS expr {}
			;

fun_decl	: ID FUN fun_decl_args block {}
			;

union_decl	: UNION ID '{' var_decls '}' {}
			;

reg_decl	: REGISTER ID '{' var_decls '}' {}
			;

var_decls	: var_decl {}
			| var_decls ',' var_decl {}
			;

fun_decl_args: '(' ')' {}
			| '(' fun_decl_args_list ')' {}
			;

fun_decl_args_list: var_decl {}
			| fun_decl_args_list ',' var_decl {}
			;


expr		: INT	{}
			| FLOAT	{}
			| STR 	{}
			| CHAR	{}
			| TRUE	{}
			| FALSE	{}
			| ID	{}
			| ID '[' expr ']' {}
			| ID ACCESS ID {}
			| ID '(' fun_call_args ')' {}
			| expr '+' expr {}
			| expr '-' expr {}
			| expr '/' expr {}
			| expr '*' expr {}
			| expr AND expr {}
			| expr OR expr	{}
			| expr '<' expr {}
			| expr '>' expr {}
			| expr GEQ expr {}
			| expr LEQ expr {}
			| expr NEQ expr {}
			| expr EQ expr {}
			| '-' expr %prec NEG {}
			| '(' expr ')'	{}
			;

fun_call_args : '(' ')' {}
			| '(' fun_call_args_lst ')' {}
			;	
	
fun_call_args_lst : expr {}
			| fun_call_args_lst ',' expr {}

block		: '{' '}' {}
			| '{' stmts '}' {}
			;

stmts		: stmt  {}
			| stmts stmt {}
			;

stmt		: ctrl_if 	{}
			| ctrl_while {}
			| ctrl_for	{}
			| var_decl '.' {}
			| var_asgn '.' {}
			| RETURN '.' {}
			| RETURN expr '.' {}
			| STOP '.' {}
			| NEXT '.' {}
			;

ctrl_if		: IF expr THEN block {}
			| IF expr THEN block ELSE block {}
			| IF expr THEN block ELSE ctrl_if {}
			;

ctrl_while	: WHILE expr DO block {}
			| DO block WHILE expr '.' {}
			;

ctrl_for	: FOR ID FROM expr TO expr block {}
			| FOR ID IN ID block {}
			;

var_asgn	: ID IS expr {}
			;




	
%%




/*
comp		: EQ {}
			| NEQ {}
			| '<' {}
			| '>' {}
			| LEQ {}
			| GEQ {}
			;
program : stmts {programBlock = $1;}

stmts	: stmt {$$ = newBlock(); $$->statements.push_back($<stmt>1);}
		| stmts stmt { $1 ->statements.push_back($<stmt>2);}
		;

stmt	: var_decl | func_decl | r

block	: 
*/

