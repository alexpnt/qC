#ifndef _SYMBOL_TABLE_
#define _SYMBOL_TABLE_

typedef enum {integer,character,arrayC,arrayI,function,program} basic_type;

typedef struct{
	char 	name[50];
	char 	type_out[50];
	int 	isFunc;
	int 	order;
}globalVar;

/*
typedef struct _value {
	basic_type type;
	union {
		int i;
		char* s;
	} data_value;
}value;*/

typedef struct{
	basic_type type;
	int 	np;
	int 	dim;
	int 	var;
}checkType;

typedef struct _table_element {
	basic_type	type;	
	char 		*name;
	int 		offset;		/*futura posição na frame caso seja uma variavel, -1 se for uma procedure.*/
	int 		order;
	char		param;
	int 		np;
	int 		dim;
	struct _table_element *next;
} table_element;

typedef struct _environment_list {
	char			*name;
	table_element	*locals;
	basic_type		returnType;
	int 			np;
	int 			prototype;
	int 			order;
	int 			used;
	struct _environment_list *next;
} environment_list;



typedef struct _prog_env {
	table_element		*global;
	environment_list	*funcs;
} prog_env;

#endif
