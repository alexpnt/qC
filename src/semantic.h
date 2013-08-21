#ifndef _SEMANTIC_
#define _SEMANTIC_

#include "symbol_table.h"

int global_offset;
int function_offset;
table_element *statTable;
prog_env *pe;

typedef enum {localScope, globalScope} scope;

void show_table();
void check_program(Node*);
void check_FuncDefinition(Node* node);
void check_funcDeclarator(Node* node,environment_list* e);
void check_funcDeclaration(Node* node);
void check_localDeclaration(Node* node,environment_list* e);
void check_globalDeclaration(Node* node);
void check_declarator(Node* node,basic_type b,table_element* t,scope sc);
void check_arrayDeclarator(Node* node,basic_type b,table_element* t,scope sc);
void check_statement(Node* node, environment_list * e);
void check_paramDeclaration(Node* node,environment_list* e);
void check_funcBody(Node* node,environment_list* e);

checkType check_expression(Node *node, environment_list *e);
checkType check_Store(Node *nodeLeft,Node *nodeRight,environment_list* e);
checkType check_AndOr(Node *,Node*,environment_list*);
checkType check_comparator(Node*,Node*,environment_list*);
checkType check_Add(Node*,Node*,environment_list*);
checkType check_Sub(Node*,Node*,environment_list*);
checkType check_DivMulMod(Node* node1,Node* node2,environment_list* e,type tipo);
checkType check_Addr(Node* node1,environment_list* e);
checkType check_Pointer(Node* node,environment_list* e);
checkType check_Array(Node* node1,Node* node2,environment_list* e);
checkType check_PlusMinus(Node* node1,environment_list* e, type tipo);
checkType check_CallFunction(Node* node,environment_list* e);
checkType check_ID(Node* node,environment_list* e);
checkType check_Int(Node* node,environment_list* e);
checkType check_Char(Node* node,environment_list* e);
checkType check_Str(Node* node,environment_list* e);

table_element* create_symbol(int offset, char* name, basic_type type,char param,int np,int dim);
table_element *lookup(table_element *table, char *str);
environment_list *lookupFunction(char *str);
table_element *lookupGlobalVariable(char *name);
int lookupGlobalFunction(char *name);
environment_list *lookupPrototype(char *name);
void insert_func(environment_list *e);
void insert_global(table_element *t);
void insert_local(environment_list *e, table_element *t);
void typeToString(table_element *element);
void returnTypeToString(basic_type tipo);
int compareqk(const void * a, const void * b);
int lookupFP(char* name);
basic_type typeToBasicType(type tipo);
void showtype(type tipo);
void typeToOperator(type tipo);

#endif
