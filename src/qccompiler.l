%{
#include "structures.h"
#include "y.tab.h"
#include <string.h>
%}
%option yylineno
%x COMMENT
var 		[a-zA-Z_]+[a-zA-Z0-9_]*
intlit 		[1-9]+[0-9]*
charlit 	[^\n"'"\\]|"\\".|\"
stringlit 	([^\\"\n]|\\.)*


%{
int col=1,colComment=1,lineComment=1;
int line=1;
%}

%%
"/*"				{lineComment=line;colComment=col;col+=2;BEGIN COMMENT;}
<COMMENT>.			col+=yyleng;
<COMMENT>"\n"		line++;col=1;
<COMMENT><<EOF>>	printf("Line %d, col %d: unterminated comment\n",lineComment,colComment);BEGIN 0;
<COMMENT>"*/"		{BEGIN 0;col+=2;}
"auto"|"break"|"case"|"const"|"continue"|"default"|"do"|"double"|"enum"|"extern"|"float"|"for"|"goto"|"long"|"register"|"short"|"signed"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"unsigned"|"void"|"volatile"|"++"|"--" {col+=yyleng;return RESERVED;}
"atoi"						{col+=yyleng;return ATOI;}
"itoa"						{col+=yyleng;return ITOA;}
"char"						{col+=yyleng;return CHAR;}
"else"						{col+=yyleng;return ELSE;}
"if"						{col+=yyleng;return IF;}
"int"						{col+=yyleng;return INT;}
"printf"					{col+=yyleng;return PRINTF;}
"return"					{col+=yyleng;return RETURN;}
"while"						{col+=yyleng;return WHILE;}
"{"							{col+=yyleng;return LBRACE;}
"}"							{col+=yyleng;return RBRACE;}
"["							{col+=yyleng;return LSQ;}
"]"							{col+=yyleng;return RSQ;}
"("							{col+=yyleng;return LPAR;}
")"							{col+=yyleng;return RPAR;}
","							{col+=yyleng;return COMMA;}
";"							{col+=yyleng;return SEMI;}
0							{col+=yyleng;yylval.value=0;return INTLIT;}
{intlit}					{col+=yyleng;yylval.value=atoi(yytext);return INTLIT;}
"'"{charlit}"'"|"'""'"		{col+=yyleng;yylval.str=(char*)strdup(yytext);return CHRLIT;}
"'"{charlit}{charlit}+"'"	{printf("Line %d, col %d: multi-character char constant\n",line, col);col+=yyleng;}
"'"{charlit}*\\?			{printf("Line %d, col %d: unterminated char constant\n",line,col);col+=yyleng;}
"\""{stringlit}"\""			{col+=yyleng;yylval.str=(char*)strdup(yytext);return STRLIT;}
\"{stringlit}\\?			{printf("Line %d, col %d: unterminated string constant\n",line,col);col+=yyleng;}
{var}						{col+=yyleng;yylval.str=(char*)strdup(yytext);return ID;}
"=" 						{col+=yyleng;return ASSIGN;}
"=="						{col+=yyleng;return EQ;}
"!="						{col+=yyleng;return NE;}
">"							{col+=yyleng;return GT;}
"<"							{col+=yyleng;return LT;}
">="						{col+=yyleng;return GE;}
"<="						{col+=yyleng;return LE;}
"+"							{col+=yyleng;return PLUS;}
"-"							{col+=yyleng;return MINUS;}
"/"							{col+=yyleng;return DIV;}
"*"							{col+=yyleng;return AST;}
"%"							{col+=yyleng;return MOD;}
"&"							{col+=yyleng;return AMP;}
"&&"						{col+=yyleng;return AND;}
"||"						{col+=yyleng;return OR;}
"!"							{col+=yyleng;return NOT;}
[ \t]						{col+=yyleng;}
\n							{col=1;line++;}
.							{printf("Line %d, col %d: illegal character ('%c')\n",line,col,yytext[0]);col+=yyleng;}


%%
int yywrap()
{
	return 1;
}
