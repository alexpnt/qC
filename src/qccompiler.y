%{
#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "functions.h"
#include "semantic.h"
#include "symbol_table.h"

void yyerror(char *);
Node* myprogram;

extern int yylineno;
extern int col;
extern char* yytext;
extern int yyleng;
%}
%token <value> INTLIT
%token <str> CHRLIT STRLIT ID CHAR INT 
%token RESERVED ATOI ITOA ELSE IF PRINTF RETURN WHILE LBRACE RBRACE LSQ RSQ LPAR RPAR COMMA SEMI ASSIGN EQ NE GT LT GE LE AST PLUS MINUS DIV MOD AMP AND OR NOT

%type <node> start
%type <node> FunctionDefinition
%type <node> NDeclaration
%type <node> FunctionDeclaration
%type <node> FunctionDeclarator
%type <node> NAST
%type <node> AST
%type <node> ParameterList
%type <node> ParameterDeclaration
%type <node> Declaration
%type <node> TypeSpecifier
%type <node> Declarator
%type <node> Statement
%type <node> NStatement
%type <node> Expression
%type <node> CommaDeclarator
%type <node> CommaExpression

%left COMMA
%right ASSIGN
%left OR
%left AND
%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left AST DIV MOD
%right NOT AMP
%nonassoc UMINUS UPLUS
%left LPAR RPAR LBRACE RBRACE LSQ RSQ
%nonassoc IF ELSE 

%union{
	char* str;
	int value;
	Node* node;
}

%%


start: FunctionDefinition 																	{$$=insert_node(Program,$1);myprogram = $$;}
		| FunctionDeclaration 																{$$=insert_node(Program,$1);myprogram = $$;}	
		| Declaration 																		{$$=insert_node(Program,$1);myprogram = $$;}		
		| start FunctionDefinition 															{$$=insert_node(None,1,$1,$2);}
		| start FunctionDeclaration 														{$$=insert_node(None,1,$1,$2);}
		| start Declaration 																{$$=insert_node(None,1,$1,$2);}
        ;

FunctionDefinition: TypeSpecifier FunctionDeclarator LBRACE RBRACE  						{$$=insert_node(FuncDefinition,1,$1,$2);}
				| TypeSpecifier FunctionDeclarator LBRACE NDeclaration RBRACE 				{$$=insert_node(FuncDefinition,2,$1,$2,$4);}
				| TypeSpecifier FunctionDeclarator LBRACE NStatement RBRACE  				{$$=insert_node(FuncDefinition,2,$1,$2,$4);}
				| TypeSpecifier FunctionDeclarator LBRACE NDeclaration NStatement RBRACE 	{$$=insert_node(FuncDefinition,3,$1,$2,$4,$5);}
				;

NDeclaration: Declaration 											{$$=$1;}
				| Declaration NDeclaration							{$$=insert_node(None,0,$1,$2);}
                 ;

NStatement: 	Statement											{$$=$1;}
				| Statement NStatement  							{$$=insert_node(None,0,$1,$2);}
                 ;

FunctionDeclaration: TypeSpecifier FunctionDeclarator SEMI 			{$$=insert_node(FuncDeclaration, $1,$2);}
					;

FunctionDeclarator: NAST ID LPAR ParameterList RPAR 				{$$=insert_node(FuncDeclarator,0,$1,$2,$4);}
					| ID LPAR ParameterList RPAR 					{$$=insert_node(FuncDeclarator,1,$1,$3);}
					| NAST ID LPAR RPAR 							{$$=insert_node(FuncDeclarator,2,$1,$2);}
					| ID LPAR RPAR 									{$$=insert_node(FuncDeclarator,3,$1);}
					;												

NAST: NAST AST     	 												{$$=insert_node(Pointer,2,$1);}
	| AST 		 													{$$=insert_node(Pointer,1);}
	;


ParameterList: ParameterDeclaration								{$$=$1;}
			| ParameterDeclaration COMMA ParameterList  			{$$=insert_node(None,0,$1,$3);}
			;

ParameterDeclaration: TypeSpecifier ID   							{$$=insert_node(ParamDeclaration,1,$1,$2);}
					| TypeSpecifier NAST ID 						{$$=insert_node(ParamDeclaration,2,$1,$2,$3);}
					;

Declaration: TypeSpecifier Declarator SEMI 							{$$=insert_node(Declaration,1,$1,$2);}
			| TypeSpecifier Declarator CommaDeclarator SEMI			{$$=insert_node(Declaration,2,$1,$2,$3);}
			;

CommaDeclarator: COMMA Declarator 									{$$ = $2; }
				| COMMA Declarator CommaDeclarator					{$$=insert_node(None,0,$2, $3);}
				; 

TypeSpecifier: CHAR 												{$$=insert_node(Char,$1);}
			| INT 													{$$=insert_node(Int,$1);}
			;

Declarator: ID LSQ INTLIT RSQ 										{$$=insert_node(ArrayDeclarator,0,$1,$3);}
			| NAST ID LSQ INTLIT RSQ								{$$=insert_node(ArrayDeclarator,1,$1,$2,$4);}
			| ID 													{$$=insert_node(Declarator,2,$1);}
			| NAST ID 												{$$=insert_node(Declarator,3,$1,$2);}
			;

Statement: Expression SEMI 											{$$=$1;}
			| SEMI 													{$$=insert_node(Ignore);}
			| LBRACE RBRACE 										{$$=insert_node(Ignore);}
			| LBRACE NStatement RBRACE 								{$$=insert_node(CompoundStat,$2);}
			| IF LPAR Expression RPAR Statement ELSE Statement 		{$$=insert_node(IfElse,3,$3,$5,$7);}
			| IF LPAR Expression RPAR Statement 					{$$=insert_node(IfElse,2,$3,$5);}
			| WHILE LPAR Expression RPAR Statement					{$$=insert_node(While,$3,$5);}
			| RETURN Expression SEMI								{$$=insert_node(Return,$2);}
			;

Expression: Expression ASSIGN Expression 					{ $$=insert_node(Store, $1, $3);}
			| Expression AND Expression 					{ $$=insert_node(And, $1, $3);}
			| Expression OR Expression						{ $$=insert_node(Or, $1, $3);}	
			| Expression EQ Expression 						{ $$=insert_node(Eq, $1, $3);}
			| Expression NE Expression 						{ $$=insert_node(Ne, $1, $3);}
			| Expression LT Expression 						{ $$=insert_node(Lt, $1, $3);}
			| Expression GT Expression						{ $$=insert_node(Gt, $1, $3);}
			| Expression LE Expression 						{ $$=insert_node(Le, $1, $3);}
			| Expression GE Expression						{ $$=insert_node(Ge, $1, $3);}
			| Expression PLUS Expression 					{ $$=insert_node(Add, $1, $3);} 
			| Expression MINUS Expression 					{ $$=insert_node(Sub, $1, $3);}
			| Expression AST Expression 					{ $$=insert_node(Mul, $1, $3);}
			| Expression DIV Expression 					{ $$=insert_node(Div, $1, $3);}
			| Expression MOD Expression 					{ $$=insert_node(Mod, $1, $3);}
			| AMP Expression 								{ $$=insert_node(Addr, $2);}
			| AST Expression 								{ $$=insert_node(Deref, 1, $2);}
			| PLUS Expression 			%prec UPLUS			{ $$=insert_node(Plus, $2);}
			| MINUS Expression			%prec UMINUS 		{ $$=insert_node(Minus, $2);}
			| NOT Expression 								{ $$=insert_node(Not, $2);}
			| Expression LSQ Expression RSQ 				{ $$=insert_node(Deref, 2, $1, $3);}   
			| ID LPAR Expression RPAR 						{ $$=insert_node(Call, 1, $1, $3);}
			| ID LPAR RPAR 	 								{ $$=insert_node(Call, 0, $1);}
			| ID LPAR Expression CommaExpression RPAR 		{ $$=insert_node(Call, 2, $1, $3, $4);}	
			| ATOI LPAR Expression RPAR 				 	{ $$=insert_node(Atoi, $3);}	
			| PRINTF LPAR Expression RPAR					{ $$=insert_node(Print, $3);}			   
			| ITOA LPAR Expression COMMA Expression RPAR  	{ $$=insert_node(Itoa, $3, $5);}
			| ID 											{ $$=insert_node(Id, $1);}
			| INTLIT 										{ $$=insert_node(IntLit, $1);}
			| CHRLIT 										{ $$=insert_node(ChrLit, $1);}
			| STRLIT 										{ $$=insert_node(StrLit, $1);}
			| LPAR Expression RPAR							{ $$ = $2;}
			;

CommaExpression : COMMA Expression 							{ $$ = $2; }
				| COMMA Expression CommaExpression			{ $$=insert_node(None,0,$2, $3);}
				;


%%
int main(int argc, char** argv)
{
	if(yyparse()!=0)
		return 0;

	if (argc == 2 && strcmp(argv[1], "-t") == 0 && myprogram){
		show(myprogram,0);
		return 0;
	}
	else if(argc == 2 && strcmp(argv[1], "-s") == 0 && myprogram){
		check_program(myprogram);
		show_table();
		return 0;
	}
	else if(argc == 3 && myprogram){
		show(myprogram,0);
		check_program(myprogram);
		show_table();
		return 0;
	}	

	if(argc==1)
		check_program(myprogram);
	printf("translating ..\n");
	/*translate(myprogram);*/
	
	return 0;
}

void yyerror (char *s) {

	if(col==yyleng)
		col++;
	else if(strcmp(yytext,"")==0)
		col+=1;
	printf ("Line %d, col %d: %s: %s\n", yylineno,col-yyleng,s,yytext);
}
