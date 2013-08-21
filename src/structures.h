#ifndef _STRUCTURES_
#define _STRUCTURES_


typedef enum{Program,																			/*No*/
			Declaration,Declarator,ArrayDeclarator,												/*Declaracao de variaveis*/
			FuncDeclaration,FuncDefinition,FuncDeclarator,FuncBody,ParamDeclaration,			/*Declaracao/Definicao de funcoes*/
			CompoundStat,IfElse,While,Return,													/*Statements*/
			Or,And,Eq,Ne,Lt,Gt,Le,Ge,Add,Sub,Mul,Div,Mod,Not,Minus,Plus,Addr,Deref,Store,Call,Print,Atoi,Itoa, /*Operadores*/
			Int,Char,Pointer,Id,IntLit,ChrLit,StrLit,											/*Terminais*/
			Null, None, Ignore} type;																			/*Especial*/

typedef union{
		int n;
		char* string;
}Dados;

typedef struct _node {
	Dados data;
	type tipo;
	struct _node * filho;
	struct _node * next;
}Node;

#endif
