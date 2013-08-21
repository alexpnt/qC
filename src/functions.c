#include "functions.h"
#include "structures.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

Node* insert_node(type tipo, ...){
	va_list ap;
	va_start(ap, tipo);
	char *aux;
	int aux3;
	int n,compound, conta;
	Node *node_aux, *node_aux2,*node_aux3,*NullNode,*CompoundNode,*node_backup;
	Node *node;

	if(tipo != None) {
		node = (Node*) malloc(sizeof(Node));
		node->tipo = tipo;
		node->filho = NULL;
		node->next = NULL;
	}

	switch (tipo) {

		case StrLit:
		case ChrLit:
		case Id:
			aux= (char*) va_arg(ap, char *);
			node->data.string= strdup(aux);
		break;
		case IntLit:
			aux3= (int) va_arg(ap, int);
			node->data.n = aux3;
		break;
		case Itoa:
			node->filho = (Node*) va_arg(ap, Node*);
			node->filho->next = (Node*) va_arg(ap, Node*);
		break;
		case Print:
		case Atoi:
			node->filho = (Node*) va_arg(ap, Node*);
		break;
		case Call:
			n=(int) va_arg(ap, int);
			node_aux = (Node*) malloc(sizeof(Node));
			node_aux->tipo = Id;
			node_aux->filho = NULL;
			node_aux->next = NULL;
			aux= (char*) va_arg(ap, char *);
			node_aux->data.string= strdup(aux);
			node->filho = node_aux;

			if(n!=0) {
				node->filho->next = (Node*) va_arg(ap, Node*); //Expression
			}
			if(n==2) {
				node_aux=node->filho->next;
				node_aux->next = (Node*) va_arg(ap, Node*); //CommaExpression
			}
		break;
		case IfElse:
			aux3=(int)va_arg(ap,int);
			conta=0;
			if(aux3==2){
				node->filho = (Node*) va_arg(ap, Node*);	//expression
				node->filho->next= (Node*) va_arg(ap, Node*);	//statement

				node_aux=node->filho->next;
				while(node_aux->next != NULL) {
					if(node_aux->tipo != Ignore) {
						conta++;
					}
					node_aux = node_aux->next;
				}
			
				if(node_aux->tipo != Ignore) {
					conta++;
				}

				if(conta==0) {
					NullNode= (Node*) malloc(sizeof(Node));
					NullNode->tipo=Null;
					NullNode->next=NULL;
					NullNode->filho=NULL;
					node_aux->next = NullNode;
				
					node_aux2= (Node*) malloc(sizeof(Node));
					node_aux2->tipo=Null;
					node_aux2->next=NULL;
					node_aux2->filho=NULL;

					node_aux->next->next = node_aux2;
				}
				else {

					node_aux2= (Node*) malloc(sizeof(Node));
					node_aux2->tipo=Null;
					node_aux2->next=NULL;
					node_aux2->filho=NULL;

					node_aux->next = node_aux2;
				}
		}
			else{	//if else aux3==3
				node->filho = (Node*) va_arg(ap, Node*); // Expression IF
				node->filho->next= (Node*) va_arg(ap, Node*);	// statement IF
				// fim do statement IF
				node_aux=node->filho->next;
				while(node_aux->next != NULL) {
					if(node_aux->tipo != Ignore) {
						conta++;
					}
					node_aux = node_aux->next;
				}

				if(node_aux->tipo != Ignore) {
					conta++;
				}

				if(conta==0) {
					NullNode= (Node*) malloc(sizeof(Node));
					NullNode->tipo=Null;
					NullNode->next=NULL;
					NullNode->filho=NULL;
					node_aux->next = NullNode;
					node_aux=node_aux->next;
				}
				conta=0;

				// ELSE
				node_aux->next=(Node*) va_arg(ap, Node*); //Statement ELSE
				node_aux2=node_aux->next;
				while(node_aux2->next != NULL) {
					if(node_aux2->tipo != Ignore) {
						conta++;
					}
					node_aux2 = node_aux2->next;
				}

				if(node_aux2->tipo != Ignore) {
					conta++;
				}

				if(conta==0) {
					node_aux3= (Node*) malloc(sizeof(Node));
					node_aux3->tipo=Null;
					node_aux3->next=NULL;
					node_aux3->filho=NULL;
					node_aux2->next = node_aux3;
				}
			}
		break;
		
		case While:
			conta=0;
			node->filho = (Node*) va_arg(ap, Node*);
			node->filho->next= (Node*) va_arg(ap, Node*);


			node_aux=node->filho->next;
			while(node_aux->next != NULL) {
				if(node_aux->tipo != Ignore) {
					conta++;
				}
				node_aux = node_aux->next;
			}
		
			if(node_aux->tipo != Ignore) {
				conta++;
			}

			if(conta==0) {
				NullNode= (Node*) malloc(sizeof(Node));
				NullNode->tipo=Null;
				NullNode->next=NULL;
				NullNode->filho=NULL;
				node_aux->next = NullNode;
			}


		break;
		case Return:
			node->filho = (Node*) va_arg(ap, Node*);
		break;
		case Not:
		case Minus:
		case Plus:
		case Addr:
			node->filho = (Node*) va_arg(ap, Node*);
		break;
		case Pointer:
			n=(int) va_arg(ap, int);

			if(n==2) {
				
				node_aux2 = (Node*) va_arg(ap, Node*);
				node_aux = node_aux2;
				while(node_aux->next != NULL) {
					node_aux=node_aux->next;
				}
				node_aux->next = node;
				return node_aux2;			
			}
		break;
		case Mod:
		case Div:
		case Mul:
		case Sub:
		case Add:
		case Ge:
		case Le:
		case Gt:
		case Lt:
		case Ne:
		case Eq:
		case Or:
		case And:
		case Store:
		case FuncDeclaration:
			node->filho = (Node*) va_arg(ap, Node*);
			node->filho->next = (Node*) va_arg(ap, Node*);
		break;
		case Deref:
			n=(int) va_arg(ap, int);
			if(n==2) {
				node_aux = (Node*) malloc(sizeof(Node));
				node_aux->tipo = Add;
				node_aux->filho = NULL;
				node_aux->next = NULL;
				node_aux->filho = (Node*) va_arg(ap, Node*);
				node_aux->filho->next = (Node*) va_arg(ap, Node*);
				node->filho = node_aux;
			}
			else {
				node->filho = (Node*) va_arg(ap, Node*);
			}
		break;
		case Declarator:
		case ArrayDeclarator:
			aux3=(int)va_arg(ap,int);						/*combinacoes*/

			node_aux = (Node*) malloc(sizeof(Node));		/*Id*/
			node_aux->tipo = Id;
			node_aux->filho = NULL;
			node_aux->next = NULL;

			node_aux2 = (Node*) malloc(sizeof(Node));		/*Intlit*/
			node_aux2->tipo = IntLit;
			node_aux2->filho = NULL;
			node_aux2->next = NULL;

			if(aux3==0){
				aux= (char*) va_arg(ap, char *);
				node_aux->data.string= strdup(aux);
				node->filho = node_aux;

				aux3=(int) va_arg(ap, int);		
				node->filho->next=node_aux2;
				node_aux2->data.n=aux3;
			}
			else if(aux3==1){
				node->filho = (Node*) va_arg(ap, Node*);

				node_aux3=node->filho;
				while(node_aux3->next != NULL) 
					node_aux3=node_aux3->next;

				aux= (char*) va_arg(ap, char *);
				node_aux->data.string= strdup(aux);
				node_aux3->next = node_aux;

				aux3=(int) va_arg(ap, int);		
				node_aux3->next->next=node_aux2;
				node_aux2->data.n=aux3;
			}
			else if(aux3==2){
				aux= (char*) va_arg(ap, char *);
				node_aux->data.string= strdup(aux);
				node->filho=node_aux;	
			}
			else{
				node->filho = (Node*) va_arg(ap, Node*);

				node_aux3=node->filho;
				while(node_aux3->next != NULL) 
					node_aux3=node_aux3->next;

				aux= (char*) va_arg(ap, char *);
				node_aux->data.string= strdup(aux);
				node_aux3->next= node_aux;
			}
		break;
		case Declaration:
			n=(int) va_arg(ap, int);
			node->filho = (Node*) va_arg(ap, Node*);
			node->filho->next = (Node*) va_arg(ap, Node*);
			if(n==2) {
				node->filho->next->next = (Node*) va_arg(ap, Node*);
			}
		break;
		case FuncDeclarator:
			aux3=(int)va_arg(ap,int);

			node_aux = (Node*) malloc(sizeof(Node));		/*Id*/
			node_aux->tipo = Id;
			node_aux->filho = NULL;
			node_aux->next = NULL;

			if(aux3==0){
				node->filho=(Node*) va_arg(ap, Node*);

				node_aux2 = node->filho;
				while(node_aux2->next != NULL)
					node_aux2 = node_aux2->next;

				aux= (char*) va_arg(ap, char *);
				node_aux->data.string= strdup(aux);

				node_aux2->next=node_aux;
				node_aux2->next->next=(Node*) va_arg(ap, Node*);
			}
			else if(aux3==1){
				aux= (char*) va_arg(ap, char *);
				node_aux->data.string= strdup(aux);

				node->filho=node_aux;
				node->filho->next=(Node*) va_arg(ap, Node*);;
			}
			else if(aux3==2){
				node->filho=(Node*) va_arg(ap, Node*);

				node_aux2 = node->filho;
				while(node_aux2->next != NULL)
					node_aux2 = node_aux2->next;

				aux= (char*) va_arg(ap, char *);
				node_aux->data.string= strdup(aux);
				node_aux2->next=node_aux;
			}
			else{
				aux= (char*) va_arg(ap, char *);
				node_aux->data.string= strdup(aux);
				node->filho=node_aux;
			}	
		break;
		case ParamDeclaration:
			n=(int) va_arg(ap, int);	// 1arg
			node->filho = (Node*) va_arg(ap, Node*); //2arg
			//ID
			node_aux = (Node*) malloc(sizeof(Node));
			node_aux->tipo = Id;
			node_aux->filho = NULL;
			node_aux->next = NULL;
			

			if(n==1) {
				aux= (char*) va_arg(ap, char *);	//3arg
				node_aux->data.string= strdup(aux);
				node->filho->next = node_aux;
			}
			else {	//Se tem NAST pelo meio
				node->filho->next = (Node*) va_arg(ap, Node*);	//3arg
				aux= (char*) va_arg(ap, char *);	//4arg
				node_aux->data.string= strdup(aux);
				//vai para o fim do NAST
				// node_aux=node->filho->next;
				node_aux2=node->filho;
				if(node_aux2==NULL)
					printf("LIXO\n");
				while (node_aux2->next != NULL) {
					node_aux2=node_aux2->next;
				}
				node_aux2->next = node_aux;
			}
		break;
		case FuncDefinition:
			n=(int) va_arg(ap, int);
			node->filho = (Node*) va_arg(ap, Node*);
			node->filho->next = (Node*) va_arg(ap, Node*);

			//FuncBody
			node_aux = (Node*) malloc(sizeof(Node));
			node_aux->tipo = FuncBody;
			node_aux->filho = NULL;
			node_aux->next = NULL;

			node->filho->next->next = node_aux;

			if(n>=2) {
				node_aux->filho = (Node*) va_arg(ap, Node*);
			}
			if(n==3) {
				node_aux2=node_aux->filho;
				while(node_aux2->next != NULL)
					node_aux2=node_aux2->next;
				node_aux2->next = (Node*) va_arg(ap, Node*);
			}
		break;
		case Program:{
			node->filho= (Node*) va_arg(ap, Node*);
			break;
		}
		case CompoundStat:
			conta=0;
			node_aux = (Node*) va_arg(ap, Node*);
			node_aux2 = node_aux;

			while(node_aux2->next!=NULL) {
				if(node_aux2->tipo != Ignore)
					conta++;
				node_aux2=node_aux2->next;
			}

			if(node_aux2->tipo != Ignore)
				conta++;


			if(conta>1) {
				node->filho = node_aux;
			}
			else {
				node = node_aux;
			}
			
		break;
		case None:
			n=(int) va_arg(ap, int);
			if(n==0) {
				node = (Node*) va_arg(ap, Node*);
				node_aux = node;
				while(node_aux->next != NULL){
					node_aux=node_aux->next;
				}
				node_aux->next = (Node*) va_arg(ap, Node*);
			}
			else {
				node = (Node*) va_arg(ap, Node*);

				node_aux = node->filho;
				while(node_aux->next != NULL)
					node_aux=node_aux->next;

				node_aux->next = (Node*) va_arg(ap, Node*);
			}

		break;
		case Ignore:
		break;
		case Null:
		break;
		default:
		break;
	}

	return node;
}

void show(Node* root,int nspaces){

	int i;
	Node *NullNode;
	char nodeTypes[46][20]={"Program","Declaration","Declarator","ArrayDeclarator","FuncDeclaration","FuncDefinition","FuncDeclarator","FuncBody","ParamDeclaration",			
			"CompoundStat","IfElse","While","Return","Or","And","Eq","Ne","Lt","Gt","Le","Ge","Add","Sub","Mul","Div","Mod","Not","Minus","Plus","Addr","Deref","Store","Call","Print","Atoi","Itoa",
			"Int","Char","Pointer","Id","IntLit","ChrLit","StrLit","Null","None","Ignore"};

	if(root->tipo!=Ignore)
		for(i=0;i<nspaces;i++)
				printf(" ");


	switch(root->tipo){
		case IntLit:{
			printf("IntLit(%d)\n",root->data.n);
			break;
		}
		case ChrLit:{
			printf("ChrLit(%s)\n",root->data.string);
			break;
		}
		case StrLit:{ 
			printf("StrLit(%s)\n",root->data.string);
			break;
		}
		case Id:
			printf("Id(%s)\n",root->data.string);
			break;
		case Ignore:
			break;
		default:
			printf("%s\n",nodeTypes[root->tipo]);
		break;
	}

	if(root->filho!=NULL){
		show(root->filho,nspaces+2);
	}
	if(root->next!=NULL)
		show(root->next,nspaces);
}
