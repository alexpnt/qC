#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structures.h"
#include "semantic.h"

#define NGLOBALS 500

prog_env *pe;
int global_order=0;
int global_offset=0;
int funtion_offset=0;
char type_out[100];

globalVar g[NGLOBALS];
int sp=0;

void show_table(){
	table_element *aux;
	environment_list *aux2, *aux3, *aux4;
	int i;

	/******************GLOBAIS*************************/
	printf("===== Global Symbol Table =====\n");			
	for (aux=pe->global; aux; aux=aux->next){
		if(!lookupGlobalVar(aux->name)){
			typeToString(aux);
			g[sp].order=aux->order;					//guarda a variavel global num vetor de variaveis globais
			g[sp].isFunc=0;
			strcpy(g[sp].name,aux->name);
			strcpy(g[sp++].type_out,type_out);
		}
	}
	for (aux2=pe->funcs; aux2; aux2=aux2->next){
		if(!lookupGlobalFunction(aux2->name)){		//se ainda nao existir este nome nas globais
			g[sp].order=aux2->order;				//guarda a funcao num vetor de variaveis globais
			g[sp].isFunc=1;
			strcpy(g[sp].name,aux2->name);
			strcpy(g[sp++].type_out,"function");
		}
	}
	qsort(g,sp,sizeof(globalVar),compareqk);		//ordena as variaveis por ordem de declaracao
	for(i=0;i<sp;i++)
		printf("%s\t%s\n",g[i].name,g[i].type_out);	//imprime as globais por ordem
	
	/******************FUNCOES****************************/
	for (aux2=pe->funcs; aux2; aux2=aux2->next){
		aux3=NULL;
		aux4=NULL;
		if( (aux2->prototype && (aux3=lookupFunction(aux2->name)) && aux2->order < aux3->order)
			|| (aux2->prototype && !lookupFunction(aux2->name))
			|| (!aux2->prototype && !lookupPrototype(aux2->name))
			|| (!aux2->prototype && (aux4=lookupPrototype(aux2->name)) && aux2->order < aux4->order)
			){
			printf("===== Function %s Symbol Table =====\n", aux2->name);
			returnTypeToString(aux2->returnType);
			printf("return\t%s",type_out);
			if(aux2->np>0){
				for(i=0;i<aux2->np;i++)
					printf("*");
			}
			printf("\n");

			if(!aux3)
				aux=aux2->locals;
			else
				aux=aux3->locals;
			
			for (; aux; aux=aux->next){
				typeToString(aux);
				printf("%s\t%s%s\n", aux->name,type_out,aux->param?"\tparam":"");
			}
		}
	}
}

void check_program(Node* program){
	int i,exist_main=0;
	Node* aux;
	environment_list* auxEnv;	

	pe = (prog_env*) malloc(sizeof(prog_env));
	pe->global = NULL;
	pe->funcs = NULL;

	for (aux=program->filho;aux; aux=aux->next){
		switch(aux->tipo){
			case FuncDefinition:
				check_FuncDefinition(aux);
				break;
			case FuncDeclaration:
				check_funcDeclaration(aux);
				break;
			case Declaration:
				check_globalDeclaration(aux);
				break;
		}
	}

	for(auxEnv=pe->funcs;auxEnv; auxEnv=auxEnv->next){
		if(auxEnv->prototype && auxEnv->used==1){
			if(lookupFunction(auxEnv->name)==NULL){
				printf("Function %s is declared and used but is not defined\n",auxEnv->name);
				exit(0);
			}
		}
	}

	for(auxEnv=pe->funcs;auxEnv; auxEnv=auxEnv->next){
		if(strcmp(auxEnv->name,"main")==0 && !auxEnv->prototype)
			return;
	}



	printf("Function main is not defined\n");
	exit(0);
}

void check_FuncDefinition(Node* node){
	Node *aux = node->filho;
	type aux_type;
	environment_list *aux2;
	table_element *aux_dec, *aux_def,*aux3;
	int i,count_dec=0, count_def=0,counter=0;

	switch(aux->tipo){				//guarda tipo
		case Int:
			aux_type=integer;
			break;
		case Char:
			aux_type=character;
			break;
	}

	environment_list *e = (environment_list*) malloc (sizeof(environment_list));
	e->next = NULL;
	e->locals = NULL;

	aux=aux->next;
	check_funcDeclarator(aux, e);

	e->returnType = aux_type;
	e->prototype = 0;
	e->order=global_order;
	if(!lookupFunction(e->name))
	{
		if(strcmp(e->name,"main")==0){
			if(e->returnType!=integer || e->np>0){
				returnTypeToString(e->returnType);
				if(e->np>0){						
					for(i=0;i<e->np;i++)
						strcat(type_out,"*");
				}	
				printf("Conflicting types in function main declaration/definition (got %s, required int)\n",type_out);
				exit(0);
			}
			else if(e->locals==NULL){
				printf("Conflicting numbers of arguments in function main declaration/definition (got 0, required 2)\n");
				exit(0);
			}
			else if(e->locals->type!=integer){
				typeToString(e->locals);
				printf("Conflicting types in function main declaration/definition (got %s, required int)\n",type_out);
				exit(0);
			}
			else if(e->locals->next==NULL){
				printf("Conflicting numbers of arguments in function main declaration/definition (got 1, required 2)\n");
				exit(0);
			}
			else if(e->locals->next->type!=arrayC || e->locals->next->np!=2){
				typeToString(e->locals->next);
				printf("Conflicting types in function main declaration/definition (got %s, required char**)\n",type_out);
				exit(0);
			}
			else{
				aux3=e->locals;
				for(;aux3 && aux3->param;aux3=aux3->next){counter++;}
				if(counter>2){
					printf("Conflicting numbers of arguments in function main declaration/definition (got %d, required 2)\n",counter);
					exit(0);
				}
			}
		}

		if( (aux2=lookupPrototype(e->name)) )  {
			if(e->returnType != aux2->returnType || e->np != aux2->np) {
				returnTypeToString(e->returnType);
				if(e->np>0){
					for(i=0;i<e->np;i++)
						strcat(type_out,"*");
				}
				printf("Conflicting types in function %s declaration/definition (got %s, required ",e->name,type_out);
				
				returnTypeToString(aux2->returnType);
				if(aux2->np>0){
					for(i=0;i<aux2->np;i++)
						strcat(type_out,"*");
				}
				printf("%s)\n",type_out);
				exit(0);
			}

			aux_dec=aux2->locals;
			aux_def=e->locals;
			for(; aux_dec && aux_def && aux_dec->param == 1 && aux_def->param == 1;) {
				if(aux_dec->param!=1)
					break;

				if(aux_dec->type != aux_def->type || aux_dec->np != aux_def->np) {
					typeToString(aux_def);
					printf("Conflicting types in function %s declaration/definition (got %s, required ", e->name, type_out);
					typeToString(aux_dec);
					printf("%s)\n", type_out);
					exit(0);
				}

				if(strcmp(aux_dec->name,aux_def->name)!=0) {
					printf("Conflicting argument names in function %s declaration/definition (got %s, required %s)\n", e->name, aux_def->name, aux_dec->name);
					exit(0);
				}

				count_dec++; count_def++;
				aux_dec=aux_dec->next;
				aux_def=aux_def->next;
			}
			
			for(; aux_def && aux_def->param == 1; aux_def=aux_def->next) {
				count_def++;
			}
			for(; aux_dec && aux_dec->param == 1; aux_dec=aux_dec->next) {
				count_dec++;
			}

			if(count_dec != count_def) {
				printf("Conflicting numbers of arguments in function %s declaration/definition (got %d, required %d)\n", e->name, count_def, count_dec);
				exit(0);	
			}
		}	

		global_order++;
		insert_func(e);
		aux=aux->next;
		check_funcBody(aux, e);
		funtion_offset++;
	}
	else{
		printf("Function %s redefined\n",e->name);
		exit(0);
	}
}


//verifica prototipo
void check_funcDeclaration(Node* node){
	Node *aux = node->filho;
	type aux_type;
	environment_list *aux2, *aux4;
	table_element *aux_dec, *aux_def,*aux3;
	int i,count_dec=0, count_def=0,counter=0;

	switch(aux->tipo){				//guarda tipo
		case Int:
			aux_type=integer;
			break;
		case Char:
			aux_type=character;
			break;
	}
	environment_list *e = (environment_list*) malloc (sizeof(environment_list));
	e->next = NULL;
	e->locals = NULL;

	aux=aux->next;
	check_funcDeclarator(aux, e);		//verifica declaracao

	e->returnType = aux_type;
	e->prototype = 1;
	e->order=global_order;


	if( (aux2=lookupPrototype(e->name)) )  {
		if(e->returnType != aux2->returnType || e->np != aux2->np) {
			returnTypeToString(e->returnType);
			if(e->np>0){
				for(i=0;i<e->np;i++)
					strcat(type_out,"*");
			}
			printf("Conflicting types in function %s declaration/definition (got %s, required ",e->name,type_out);
			
			returnTypeToString(aux2->returnType);
			if(aux2->np>0){
				for(i=0;i<aux2->np;i++)
					strcat(type_out,"*");
			}
			printf("%s)\n",type_out);
			exit(0);
		}

		aux_dec=aux2->locals;
		aux_def=e->locals;
		for(; aux_dec && aux_def && aux_dec->param == 1 && aux_def->param == 1;) {
			if(aux_dec->param!=1)
				break;

			if(aux_dec->type != aux_def->type || aux_dec->np != aux_def->np) {
				typeToString(aux_def);
				printf("Conflicting types in function %s declaration/definition (got %s, required ", e->name, type_out);
				typeToString(aux_dec);
				printf("%s)\n", type_out);
				exit(0);
			}

			if(strcmp(aux_dec->name,aux_def->name)!=0) {
				printf("Conflicting argument names in function %s declaration/definition (got %s, required %s)\n", e->name, aux_def->name, aux_dec->name);
				exit(0);
			}

			count_dec++; count_def++;
			aux_dec=aux_dec->next;
			aux_def=aux_def->next;
		}
		
		for(; aux_def && aux_def->param == 1; aux_def=aux_def->next) {
			count_def++;
		}
		for(; aux_dec && aux_dec->param == 1; aux_dec=aux_dec->next) {
			count_dec++;
		}

		if(count_dec != count_def) {
			printf("Conflicting numbers of arguments in function %s declaration/definition (got %d, required %d)\n", e->name, count_def, count_dec);
			exit(0);	
		}
	}




	if(!lookupPrototype(e->name)){

		if(strcmp(e->name,"main")==0){
			if(e->returnType!=integer || e->np>0){
				returnTypeToString(e->returnType);
				if(e->np>0){						
					for(i=0;i<e->np;i++)
						strcat(type_out,"*");
				}	
				printf("Conflicting types in function main declaration/definition (got %s, required int)\n",type_out);
				exit(0);
			}
			else if(e->locals==NULL){
				printf("Conflicting numbers of arguments in function main declaration/definition (got 0, required 2)\n");
				exit(0);
			}
			else if(e->locals->type!=integer){
				typeToString(e->locals);
				printf("Conflicting types in function main declaration/definition (got %s, required int)\n",type_out);
				exit(0);
			}
			else if(e->locals->next==NULL){
				printf("Conflicting numbers of arguments in function main declaration/definition (got 1, required 2)\n");
				exit(0);
			}
			else if(e->locals->next->type!=arrayC || e->locals->next->np!=2){
				typeToString(e->locals->next);
				printf("Conflicting types in function main declaration/definition (got %s, required char**)\n",type_out);
				exit(0);
			}
			else{
				aux3=e->locals;
				for(;aux3 && aux3->param;aux3=aux3->next){counter++;}
				if(counter>2){
					printf("Conflicting numbers of arguments in function main declaration/definition (got %d, required 2)\n",counter);
					exit(0);
				}
			}
		}
		
		aux_dec=NULL; aux_def=NULL;
		count_dec=count_def=0;

		if( (aux2=lookupFunction(e->name)) )  {
			if(e->returnType != aux2->returnType || e->np != aux2->np) {
				returnTypeToString(e->returnType);
				if(e->np>0){
					for(i=0;i<e->np;i++)
						strcat(type_out,"*");
				}
				printf("Conflicting types in function %s declaration/definition (got %s, required ",e->name,type_out);
				
				returnTypeToString(aux2->returnType);
				if(aux2->np>0){
					for(i=0;i<aux2->np;i++)
						strcat(type_out,"*");
				}
				printf("%s)\n",type_out);
				exit(0);
			}

			aux_dec=aux2->locals;
			aux_def=e->locals;
			for(; aux_dec && aux_def && aux_dec->param == 1 && aux_def->param == 1;) {
				if(aux_dec->param!=1)
					break;
				
				if(aux_dec->type != aux_def->type || aux_dec->np != aux_def->np) {
					typeToString(aux_def);
					printf("Conflicting types in function %s declaration/definition (got %s, required ", e->name, type_out);
					typeToString(aux_dec);
					printf("%s)\n", type_out);
					exit(0);
				}

				if(strcmp(aux_dec->name,aux_def->name)!=0) {
					printf("Conflicting argument names in function %s declaration/definition (got %s, required %s)\n", e->name, aux_def->name, aux_dec->name);
					exit(0);
				}

				count_dec++; count_def++;
				aux_dec=aux_dec->next;
				aux_def=aux_def->next;
			}
			
			for(; aux_def && aux_def->param == 1; aux_def=aux_def->next) {
				count_def++;
			}
			for(; aux_dec && aux_dec->param == 1; aux_dec=aux_dec->next) {
				count_dec++;
			}
			if(count_dec != count_def) {
				printf("Conflicting numbers of arguments in function %s declaration/definition (got %d, required %d)\n", e->name, count_def, count_dec);
				exit(0);	
			}
		}

		global_order++;
		insert_func(e);					//guarda prototipo
		funtion_offset++;
	}
}


void check_funcBody(Node* node, environment_list *e){
	Node *aux = node->filho;

	for(; aux; aux=aux->next) {
		switch(aux->tipo) {
			case Declaration:
				check_localDeclaration(aux,e);
				break;
			default:
				check_statement(aux,e);
				break;	
		}
	}
}

void check_globalDeclaration(Node* node){
	Node *aux = node->filho;
	basic_type tipo;

	table_element *te;

	tipo = typeToBasicType(aux->tipo);

	for(aux=aux->next; aux; aux=aux->next) {
		switch (aux->tipo) {
			case Declarator:
				te = (table_element*) malloc (sizeof(table_element));
				te->next = NULL;
				te->dim=0;
				te->param=0;
				te->np=0;
				check_declarator(aux, tipo, te,globalScope);
				te->order=global_order;
				insert_global(te);
				break;
			case ArrayDeclarator:
				te = (table_element*) malloc (sizeof(table_element));
				te->next = NULL;
				te->dim=0;
				te->param=0;
				te->np=0;
				check_arrayDeclarator(aux, tipo, te,globalScope);
				te->order=global_order;
				if(te->dim<=0){
					printf("Size of array %s is not positive\n",te->name);
					exit(0);
				}
				insert_global(te);
				break;
		}
	}
}

void check_localDeclaration(Node* node, environment_list *e){
	Node *aux = node->filho;
	basic_type tipo;

	table_element *te;

	tipo = typeToBasicType(aux->tipo);

	for(aux=aux->next; aux; aux=aux->next) {
		switch (aux->tipo) {
			case Declarator:
				te = (table_element*) malloc (sizeof(table_element));
				te->next = NULL;
				te->dim=0;
				te->param=0;
				te->np=0;
				check_declarator(aux, tipo, te,localScope);
				insert_local(e, te);
				break;
			case ArrayDeclarator:
				te = (table_element*) malloc (sizeof(table_element));
				te->next = NULL;
				te->dim=0;
				te->param=0;
				te->np=0;
				check_arrayDeclarator(aux, tipo, te,localScope);
				if(te->dim<=0){
					printf("Size of array %s is not positive\n",te->name);
					exit(0);
				}
				insert_local(e, te);
				break;
		}
	}
}

void check_declarator(Node* node,basic_type b,table_element* t,scope sc){

	Node *aux = node->filho;

	for(; aux; aux=aux->next) {
		switch(aux->tipo) {
			case Pointer:
				t->np++; 
				break;
			case Id:
				t->name = strdup(aux->data.string);
				break;
		}
	}

	if(t->np>0 && b==integer){
		t->type=arrayI;
	}
	else if(t->np>0 && b==character){
		t->type=arrayC;
	}
	else if(t->np==0 && b==integer){
		t->type=integer;
	}
	else if(t->np==0 && b==character){
		t->type=character;
	}

	if(sc==localScope)
		t->offset=funtion_offset;
	else
		t->offset=global_offset;
}

void check_arrayDeclarator(Node* node,basic_type b,table_element* t,scope sc){

	Node *aux = node->filho;

	for(; aux; aux=aux->next) {
		switch(aux->tipo) {
			case Pointer:
				t->np++; 
				break;
			case Id:
				t->name = strdup(aux->data.string);
				break;
			case IntLit:
				t->dim=aux->data.n;
		}
	}

	if(b==integer){
		t->type=arrayI;
	}
	else if(b==character){
		t->type=arrayC;
	}

	if(sc==localScope)
		t->offset=funtion_offset;
	else
		t->offset=global_offset;
}

void check_statement(Node* node, environment_list * e){
	Node *aux=node;
	//showtype(node->tipo);
	
	switch(node->tipo) {
		case (CompoundStat):
			for(aux=node->filho; aux; aux=aux->next) {
				check_statement(aux,e);
			}
			break;

		case (IfElse):
			check_expression(aux->filho,e);
			aux=aux->filho->next;
			check_statement(aux,e);
			if(aux->next)
				check_statement(aux->next, e);
			break;

		case (While):
			check_expression(aux->filho,e);
			aux=aux->filho->next;
			check_statement(aux,e);
			break;

		case (Return):
			check_expression(aux->filho,e);
			break;
		default:
			check_expression(aux,e);
			break;
	}
}

checkType check_expression(Node *node, environment_list *e){
	switch(node->tipo){
		case Store:
			return check_Store(node->filho,node->filho->next,e);
			break;
		case And:case Or: case Itoa:
			check_expression(node->filho,e);check_expression(node->filho->next,e);
			break;
		case Eq:case Ne:case Lt:case Gt:case Le:case Ge:
			return check_comparator(node->filho,node->filho->next,e);
			break;
		case Add:
			return check_Add(node->filho,node->filho->next,e);
			break;
		case Sub:
			return check_Sub(node->filho,node->filho->next,e); 
			break;
		case Div:case Mul:case Mod:
			return check_DivMulMod(node->filho,node->filho->next,e,node->tipo);
			break;
		case Addr:
			return check_Addr(node->filho,e);
			break;
		case Deref:
			if(node->filho->tipo != Add)
				return check_Pointer(node->filho,e);
			else{
				return check_Array(node->filho->filho,node->filho->filho->next,e);
			}
			break;
		case Plus:case Minus:
			return check_PlusMinus(node->filho,e,node->tipo);
			break;
		case Call:	
			return check_CallFunction(node->filho,e);
			break;
		case Not: case Atoi:case Print:
			return check_expression(node->filho,e);
			break;
		case Id:			
			return check_ID(node,e);
			break;
		case IntLit:
			return check_Int(node->filho,e);
			break;
		case ChrLit:
			return check_Char(node->filho,e);
			break;
		case StrLit:
			return check_Str(node->filho,e);
			break;
		default:
			break;
	}
}

checkType check_Store(Node *nodeLeft,Node *nodeRight,environment_list* e){

	checkType return1,return2;
	/*int dim = return1.dim;*/
	return1=check_expression(nodeLeft,e);
	/*if(nodeLeft->tipo == Deref) {
		dim = return1.dim;
		return1.dim=0;
	}*/
	return2=check_expression(nodeRight,e);

	if(return1.var!=1 || return1.dim>0){
		printf("Lvalue required\n");
		exit(0);
	}
	/*return1.dim = dim;*/
	return return1;
}

checkType check_comparator(Node* node1,Node* node2,environment_list* e){
	checkType return1;
	return1.np=0;
	return1.dim=0;

	check_expression(node1,e);
	check_expression(node2,e);

	return1.type=integer;
	return return1;
}

checkType check_Add(Node* node1,Node* node2,environment_list* e){

	checkType return1,return2;
	int i;

	return1=check_expression(node1,e);
	return2=check_expression(node2,e);

	if((return1.np>0 || return1.dim>0) && (return2.np>0 || return2.dim>0)){
		returnTypeToString(return1.type);
		printf("Operator + cannot be applied to types %s",type_out);
		for(i=0;i<return1.np;i++)
			printf("*");
		if(return1.dim>0){
			printf("[%d]",return1.dim);
		}
		returnTypeToString(return2.type);
		printf(", %s",type_out);
		for(i=0;i<return2.np;i++)
			printf("*");
		if(return2.dim>0){
			printf("[%d]",return2.dim);
		}
		printf("\n");
		exit(0);
	}

	if(return1.np>return2.np || return1.dim>return2.dim)
		return return1;
	else
		return return2;
}

checkType check_Sub(Node* node1,Node* node2,environment_list* e){

	checkType return1,return2;
	int i;

	return1=check_expression(node1,e);
	return2=check_expression(node2,e);

	if(((return1.dim>0 || return1.np>0) && (return2.dim<=0 && return2.np<=0) ) || ((return2.dim>0 || return2.np>0) && (return1.dim<=0 && return1.np<=0))){
		returnTypeToString(return1.type);
		printf("Operator - cannot be applied to types %s",type_out);
		for(i=0;i<return1.np;i++)
			printf("*");
		if(return1.dim>0)
			printf("[%d]",return1.dim);
		returnTypeToString(return2.type);
		printf(", %s",type_out);
		for(i=0;i<return2.np;i++)
			printf("*");
		if(return2.dim>0)
			printf("[%d]",return2.dim);
		printf("\n");
		exit(0);
	}
	if((return1.var!=1 && return2.var!=1 && return1.type!=return2.type) || (return1.np!=return2.np)){
		returnTypeToString(return1.type);
		printf("Operator - cannot be applied to types %s",type_out);
		for(i=0;i<return1.np;i++)
			printf("*");
		if(return1.dim>0)
			printf("[%d]",return1.dim);
		returnTypeToString(return2.type);
		printf(", %s",type_out);
		for(i=0;i<return2.np;i++)
			printf("*");
		if(return2.dim>0)
			printf("[%d]",return2.dim);
		printf("\n");
		exit(0);
	}

	return return1;

}

checkType check_DivMulMod(Node* node1,Node* node2,environment_list* e, type tipo){
	checkType return1,return2;
	int i;

	return1=check_expression(node1,e);
	return2=check_expression(node2,e);
	
	if((return1.np)>0 || (return2.np)>0 || return1.dim>0 || return2.dim>0){
		typeToOperator(tipo);
		printf("Operator %s cannot be applied to types ",type_out);
		returnTypeToString(return1.type);
		printf("%s",type_out);
		for(i=0;i<return1.np;i++)
			printf("*");
		if(return1.dim>0)
			printf("[%d]",return1.dim);
		returnTypeToString(return2.type);
		printf(", %s",type_out);
		for(i=0;i<return2.np;i++)
			printf("*");
		if(return2.dim>0)
			printf("[%d]",return2.dim);
		printf("\n");
		exit(0);
	}
	return return1;
}

checkType check_Addr(Node* node1,environment_list* e){

	checkType return1;

	return1=check_expression(node1,e);

	if(return1.var!=1){
		printf("Lvalue required\n");
		exit(0);
	}

	(return1.np)++;
	return return1;
}

checkType check_Pointer(Node* node,environment_list* e){

	checkType return1;
	return1=check_expression(node,e);
	return1.np--;

	if(return1.var!=1){
		printf("Lvalue required\n");
		exit(0);
	}

	if(return1.np<0){
		returnTypeToString(return1.type);
		printf("Operator * cannot be applied to type %s\n",type_out);
		exit(0);
	}

	return return1;
}

checkType check_Array(Node* node1,Node* node2,environment_list* e){

	checkType return1,return2,return3;
	return1=check_expression(node1,e);

	if(return1.var!=1 || ((return1.dim<=0) && (return1.np<=0))){
		printf("Operator * cannot be applied to type int\n");
		exit(0);
	}

	return2=check_expression(node2,e);
	
	if(return2.np>0 || return2.dim>0){
		printf("Operator * cannot be applied to type int\n");
		exit(0);
	}


	//return1.type=exp->operation;
	return1.var=1;
	return1.dim=0;
	return return1;

}

checkType check_PlusMinus(Node* node,environment_list* e, type tipo){
	checkType return1;
	return1.np=0;
	return1.dim=0;
	int i;

	return1=check_expression(node, e);

	if((return1.type!=character && return1.type!=integer) || (return1.np)>0 || (return1.dim)>0){
		typeToOperator(tipo);
		printf("Operator %s cannot be applied to type ",type_out);
		returnTypeToString(return1.type);
		printf("%s",type_out);
		for(i=0;i<return1.np;i++)
			printf("*");
		if((return1.dim)>0)
			printf("[%d]",return1.dim);
		printf("\n");
		exit(0);
	}

	return return1;
}

checkType check_CallFunction(Node* node,environment_list* e){

	checkType return1;
	return1.np=0;
	return1.dim=0;
	environment_list* func,*prot; 
	table_element* args;
	Node* node_aux;
	int nArgs=0,nArgsReq;

	/*procura nas variaveis locais*/
	if(lookup(e->locals,node->data.string) || lookupGlobalVariable(node->data.string)){
		printf("Symbol %s is not a function\n",node->data.string);
		exit(0);
	}
	
	func=lookupFunction(node->data.string);
	prot=lookupPrototype(node->data.string);

	if(!func && !prot){
		printf("Unknown symbol %s\n",node->data.string);
		exit(0);
	}
	if(!func)
		func=prot;
	if(func || prot){
		//printf("existe\n");
		for(args=func->locals,node_aux=node->next;
			args && args->param && node_aux;
			node_aux=node_aux->next,args=args->next,nArgs++)
		{
			check_expression(node_aux,e);
		}

		if(args!=NULL){
			nArgsReq=nArgs;
	
			for(;args && args->param;args=args->next)
				nArgs++;
			if(nArgsReq!=nArgs){
				printf("Wrong number of arguments in call to function %s (got %d, required %d)\n",func->name,nArgsReq,nArgs);
				exit(0);
			}
		}else if(node_aux!=NULL){
			nArgsReq=nArgs;

			for(;node_aux;node_aux=node_aux->next){
				check_expression(node_aux,e);
				nArgs++;
			}
			if(nArgsReq!=nArgs){
				printf("Wrong number of arguments in call to function %s (got %d, required %d)\n",func->name,nArgs,nArgsReq);
				exit(0);
			}
		}
		
		if(prot)
			prot->used=1;
	}
	return1.type=func->returnType;
	return1.np=func->np;

	return return1;
}

checkType check_ID(Node* node,environment_list* e){

	checkType return1;
	return1.np=0;
	return1.dim=0;
	table_element *local;
	environment_list *globalFunc,*globalProt;

	if((local=lookup(e->locals,node->data.string))==NULL){
		if((local=lookupGlobalVariable(node->data.string))==NULL){
			if((globalFunc=lookupFunction(node->data.string))==NULL && (globalProt=lookupPrototype(node->data.string))==NULL){
				printf("Unknown symbol %s\n",node->data.string);
				exit(0);
			}
		}
	}


	if(local) {
		return1.type=local->type;
		return1.np=local->np;
		return1.dim=local->dim;
		return1.var=1;
	}
	else
		return1.var=2;

	return return1;
}
checkType check_Int(Node* node,environment_list* e){
	checkType return1;
	return1.np=0;
	return1.dim=0;
	return1.type=integer;
	return return1;
}
checkType check_Char(Node* node,environment_list* e){
	checkType return1;
	return1.np=0;
	return1.dim=0;
	return1.type=character;
	return return1;
}	
checkType check_Str(Node* node,environment_list* e){
	checkType return1;
	return1.type=character;
	return1.dim=0;
	return1.np=1;
	return return1;
}	

void check_funcDeclarator(Node* node, environment_list *e) {
	Node *aux = node->filho;
	int np=0;

	for(; aux; aux=aux->next) {
		switch(aux->tipo) {
			case Pointer:
				np++; 
				break;
			case Id:
				e->name = strdup(aux->data.string);
				break;
			case ParamDeclaration:
				check_paramDeclaration(aux, e);
		}
	}
	e->np = np;
}

void check_paramDeclaration(Node* node, environment_list *e) {
	Node *aux = node->filho;
	int np=0;
	table_element *te = (table_element*) malloc (sizeof(table_element));
	te->next = NULL;
	te->dim=0;
	te->offset = funtion_offset;
	te->param  = 1;

	for(;aux;aux=aux->next) {
		switch (aux->tipo) {
			case Pointer:
				np++;	
				break;
			case Id:
				te->name = strdup(aux->data.string);
				break;
		}
	}

	te->np = np;

	if(te->np>0 && node->filho->tipo==Int){
		te->type=arrayI;
	}
	else if(te->np>0 && node->filho->tipo==Char){
		te->type=arrayC;
	}
	else if(te->np==0 && node->filho->tipo==Int){
		te->type=integer;
	}
	else if(te->np==0 && node->filho->tipo==Char){
		te->type=character;
	}


	insert_local(e, te);
}

basic_type typeToBasicType(type tipo) {

	basic_type ret;

	switch (tipo) {
		case Int:
			ret = integer;
			break;
		case Char:
			ret = character;
	}

	return ret;
}

void typeToString(table_element *element){			
	int i;
	char buffer[10];

	switch(element->type){
		case integer:
			strcpy(type_out,"int");
			break;
		case character:
			strcpy(type_out,"char");
			break;
		case arrayC:
			strcpy(type_out,"char");
			if(element->np>0){						
				for(i=0;i<element->np;i++)
					strcat(type_out,"*");
			}
			if(element->dim>0){					
				strcat(type_out,"[");
				sprintf(buffer,"%d",element->dim);
				strcat(type_out,buffer);
				strcat(type_out,"]");
			}
			break;
		case arrayI:
			strcpy(type_out,"int");
			if(element->np>0){
				for(i=0;i<element->np;i++)
					strcat(type_out,"*");
			}
			if(element->dim>0){
				strcat(type_out,"[");
				sprintf(buffer,"%d",element->dim);
				strcat(type_out,buffer);
				strcat(type_out,"]");
			}
			break;
	}	
}


void returnTypeToString(basic_type tipo) {
	switch(tipo){				//guarda tipo
		case integer: case arrayI:
			strcpy(type_out,"int");
			break;
		case character: case arrayC:
			strcpy(type_out,"char");
			break;
	}
}


table_element* create_symbol(int offset, char* name, basic_type type,char param,int np,int dim){
	table_element* el=(table_element*)malloc(sizeof(table_element));
	el->type=type;
	el->name=strdup(name);
	el->offset=offset;
	el->param=param;
	el->np=np;
	el->dim=dim;
	el->next=NULL;
	return el;
}

table_element *lookup(table_element* table, char *name)
{
	table_element *aux;

	for(aux=table; aux; aux=aux->next)
		if(strcmp(aux->name, name)==0)
			return aux;

	return NULL;
}

environment_list *lookupFunction(char *name) {
	environment_list *aux;
	
	for (aux=pe->funcs; aux; aux=aux->next) {
		if (strcmp(aux->name, name) == 0 && !(aux->prototype)) {
			return aux;
		}
	}	
	return NULL;
}

int lookupGlobalFunction(char *name){

	int i;
	
	for(i=0;i<sp;i++){
		if(strcmp(g[i].name,name)==0 && g[i].isFunc==1)
			return 1;
	}

	return 0;
}

int lookupGlobalVar(char *name){

	int i;
	
	for(i=0;i<sp;i++){
		if(strcmp(g[i].name,name)==0)
			return 1;
	}

	return 0;
}

table_element *lookupGlobalVariable(char *name)
{
	table_element *aux;

	for(aux=pe->global; aux; aux=aux->next)
		if(strcmp(aux->name, name)==0)
			return aux;

	return NULL;
}

environment_list *lookupPrototype(char *name){
	environment_list *aux;
	
	for (aux=pe->funcs; aux; aux=aux->next) {
		if (strcmp(aux->name, name) == 0 && aux->prototype) {
			return aux;
		}
	}
	return NULL;
}


void insert_local(environment_list *e, table_element *t) {
	table_element *aux;

	for(aux=e->locals; aux; aux=aux->next) {
		if(strcmp(aux->name,t->name)==0){
				printf("Symbol %s redefined\n", t->name);
				exit(0);
		}
	}

	if(!e->locals)
		e->locals = t;
	else {
		for(aux=e->locals; aux->next; aux=aux->next);
		aux->next=t;
	}
}

void insert_func(environment_list *e) {
	environment_list *aux;

	if(!pe->funcs)
		pe->funcs=e;
	else {
		for(aux=pe->funcs; aux->next; aux=aux->next);
		aux->next=e;
	}

}

void insert_global(table_element *t) {
	table_element *aux;

	for(aux=pe->global; aux; aux=aux->next) {
		if(strcmp(aux->name,t->name)==0){
			if(aux->type != t->type) {
				printf("Symbol %s redefined\n", t->name);
				exit(0);
			}
			else
				return;
		}
	}

	if(!pe->global)
		pe->global=t;
	else {
		for(aux=pe->global; aux->next; aux=aux->next);
		aux->next=t;
		global_order++;
		global_offset++;
	}
}

void showtype(type tipo){

	char nodeTypes[46][20]={"Program","Declaration","Declarator","ArrayDeclarator","FuncDeclaration","FuncDefinition","FuncDeclarator","FuncBody","ParamDeclaration",			
			"CompoundStat","IfElse","While","Return","Or","And","Eq","Ne","Lt","Gt","Le","Ge","Add","Sub","Mul","Div","Mod","Not","Minus","Plus","Addr","Deref","Store","Call","Print","Atoi","Itoa",
			"Int","Char","Pointer","Id","IntLit","ChrLit","StrLit","Null","None","Ignore"};

	printf("%s\n",nodeTypes[tipo]);

}

int compareqk(const void *a, const void *b)			/*comparador do quicksort*/
{													
	globalVar* g1=(globalVar*)a;
	globalVar* g2=(globalVar*)b;

	return g1->order-g2->order;
}

void typeToOperator(type tipo) {

	switch(tipo) {
		case Mul: strcpy(type_out,"*");		break;
		case Div: strcpy(type_out,"/");		break;
		case Mod: strcpy(type_out,"%");		break;
		case Plus: strcpy(type_out,"+");	break;
		case Minus: strcpy(type_out,"-");	break;

	}
}

