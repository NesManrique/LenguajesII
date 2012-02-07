%{
#include "ast.h"
#include <cstdio>
extern int yylex (void);
void yyerror (char const *a){printf("ERROR: %s\n",a);};

NBlock *ProgramAST;

%}

/* Ways to access data */

%union{
	Node *node;
	NBlock	*block;
	NExpression *expr;
	NStatement *stmt;
	NIdentifier	*ident;
	NVariableDeclaration *var_decl;
	NArrayDeclaration *arr_decl;
	NArrayAccess *arr_access;
	NRegisterDeclaration *reg_decl;
	NUnionDeclaration *union_decl;
	std::vector<NVariableDeclaration*> *varvec;
	std::vector<NExpression*> *exprvec;
	std::string *string;
	int token;
	long long integer;
	double floating;
	char Char;

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
%type	<ident>	ident
%type	<varvec> fun_decl_args var_decls fun_decl_args_list
%type	<exprvec>	fun_call_args fun_call_args_lst /*array_elem*/
%type	<block>	program stmts block decls 
%type	<stmt>	stmt var_decl fun_decl reg_decl 
%type	<stmt>	union_decl ctrl_while ctrl_if
/*%type	<token>	comparison*/

/* Matematical operators precedence */
%nonassoc <token>	EQ NEQ GEQ LEQ '<' '>'	
%left	<token>	'+' '-' AND OR
%left	<token> '*' '/'
%left 	<token> NEG NOT
%left	<token> ACCESS

%start program
%%
program 	: decls {ProgramAST = $1;}
			;

decls		: decl	{$$ = new NBlock();$$->statements.push_back($<stmt>1);}
			| decls decl {$$->statements.push_back($<stmt>2);}
			;

decl		: var_decl '.' 
			| reg_decl 
			| union_decl 
			| fun_decl 
			;

var_decl	: ident ident {$$ = new NVariableDeclaration(*$1,*$2);}
			| ident ident '=' expr {$$ = new NVariableDeclaration(*$1,*$2,$4);}
			;

fun_decl	: ident FUN ident fun_decl_args block {$$ = new NFunctionDeclaration(*$1,*$3,*$4,*$5);}
			;

union_decl	: UNION ident '{' var_decls '}' {$$ = new NUnionDeclaration(*$2,*$4);}
			;

reg_decl	: REGISTER ident '{' var_decls '}' {$$ = new NRegisterDeclaration(*$2,*$4);}
			;

var_decls	: var_decl {$$ = new VariableList();$$->push_back($<var_decl>1);}
			| var_decls ',' var_decl {$$->push_back($<var_decl>3);}
			;

fun_decl_args: '(' ')' {$$ = new VariableList();}
			| '(' fun_decl_args_list ')' {$$ = $2;}
			;

fun_decl_args_list: var_decl {$$ = new VariableList();$$->push_back($<var_decl>1);}
			| fun_decl_args_list ',' var_decl {$$->push_back($<var_decl>3);}
			;

ident		: ID {$$ = new NIdentifier(*$1);}


expr		: INT	{$$ = new NInteger($1);}	
			| FLOAT	{$$ = new NDouble($1);}
			| STR 	{$$ = new NString(*$1);}
			| CHAR	{$$ = new NChar($1);}	
			| TRUE	{$$=new NBool(true);}
			| FALSE	{$$=new NBool(false);}
			| ident	{$$=new NIdentifier(*$1);}
			| ident '[' expr ']' {$$=new NArrayAccess(*$1,*$3);}
			| reg_access 	{}
			| ident  fun_call_args  {$$ = new NFunctionCall(*$1,*$2);}
			| expr '+' expr {$$=new NBinaryOperator(*$1,$2,*$3);}
			| expr '-' expr {$$=new NBinaryOperator(*$1,$2,*$3);}
			| expr '/' expr {$$=new NBinaryOperator(*$1,$2,*$3);}
			| expr '*' expr {$$=new NBinaryOperator(*$1,$2,*$3);}
			| expr AND expr {$$=new NBinaryOperator(*$1,$2,*$3);}
			| expr OR expr	{$$=new NBinaryOperator(*$1,$2,*$3);}
			| expr '<' expr {$$=new NBinaryOperator(*$1,$2,*$3);}
			| expr '>' expr {$$=new NBinaryOperator(*$1,$2,*$3);}
			| expr GEQ expr {$$=new NBinaryOperator(*$1,$2,*$3);}
			| expr LEQ expr {$$=new NBinaryOperator(*$1,$2,*$3);}
			| expr NEQ expr {$$=new NBinaryOperator(*$1,$2,*$3);}
			| expr EQ expr {$$=new NBinaryOperator(*$1,$2,*$3);}
			| '-' expr %prec NEG {$$=new NUnaryOperator($1,*$2);}
			| '(' expr ')'	{$$=$2;}
			;

reg_access	:	ident ACCESS ident {}
			|	reg_access ACCESS ident {}
			;


fun_call_args : '(' ')' {$$= new ExpressionList();}
			| '(' fun_call_args_lst ')' {$$=$2;}
			;	
	
fun_call_args_lst : expr {$$=new ExpressionList();$$->push_back($1);}
			| fun_call_args_lst ',' expr {$$->push_back($3);}

block		: '{' '}' {$$ = new NBlock();}
			| '{' stmts '}' {$$ =$2;}
			;

stmts		: stmt  {$$ = new NBlock();$$->statements.push_back($1);}
			| stmts stmt {$$->statements.push_back($2);}
			;

stmt		: ctrl_if 	
			| ctrl_while 
			| ctrl_for	{}
			| block 	{$$=$<stmt>1;}
			| var_decl '.' 
			| var_asgn '.' {}
			| ident  fun_call_args  '.' {}
			| RETURN '.' {$$ = new NReturn();}
			| RETURN expr '.' {$$ = new NReturn($2);}
			| STOP '.' {$$ = new NStop();}
			| NEXT '.' {$$ = new NNext();}
			;

ctrl_if		: IF expr THEN block {$$ = new NIf($2,$4);}
			| IF expr THEN block ELSE block {$$ = new NIf($2,$4,$6);}
			| IF expr THEN block ELSE ctrl_if {$$ = new NIf($2,$4,$6);}
			;

ctrl_while	: WHILE expr DO block {$$ = new NWhileDo($2,*$4);}
			| DO block WHILE expr '.' {$$ = new NDoWhile($4,*$2);}
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

