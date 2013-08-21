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
"auto"|"break"|"case"|"const"|"continue"|"default"|"do"|"double"|"enum"|"extern"|"float"|"for"|"goto"|"long"|"register"|"short"|"signed"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"unsigned"|"void"|"volatile" {printf("RESERVED\n");col+=yyleng;}
"atoi"						{printf("ATOI\n");col+=yyleng;}
"itoa"						{printf("ITOA\n");col+=yyleng;}
"char"						{printf("CHAR\n");col+=yyleng;}
"else"						{printf("ELSE\n");col+=yyleng;}
"if"						{printf("IF\n");col+=yyleng;}
"int"						{printf("INT\n");col+=yyleng;}
"printf"					{printf("PRINTF\n");col+=yyleng;}
"return"					{printf("RETURN\n");col+=yyleng;}
"while"						{printf("WHILE\n");col+=yyleng;}
"{"							{printf("LBRACE\n");col+=yyleng;}
"}"							{printf("RBRACE\n");col+=yyleng;}
"["							{printf("LSQ\n");col+=yyleng;}
"]"							{printf("RSQ\n");col+=yyleng;}
"("							{printf("LPAR\n");col+=yyleng;}
")"							{printf("RPAR\n");col+=yyleng;}
","							{printf("COMMA\n");col+=yyleng;}
";"							{printf("SEMI\n");col+=yyleng;}
0							{printf("INTLIT(0)\n");col+=yyleng;}
{intlit}					{printf("INTLIT(%s)\n",yytext);col+=yyleng;}
"'"{charlit}"'"|"'""'"		{printf("CHRLIT(%s)\n",yytext);col+=yyleng;}
"'"{charlit}{charlit}+"'"	{printf("Line %d, col %d: multi-character char constant\n",line, col);col+=yyleng;}	
"'"{charlit}*\\?			{printf("Line %d, col %d: unterminated char constant\n",line,col,yytext);col+=yyleng;}
"\""{stringlit}"\""			{printf("STRLIT(%s)\n",yytext);col+=yyleng;}
\"{stringlit}\\?				{printf("Line %d, col %d: unterminated string constant\n",line,col,yytext);col+=yyleng;}
{var}						{printf("ID(%s)\n",yytext);col+=yyleng;}
"=" 						{printf("ASSIGN\n");col+=yyleng;}
"=="						{printf("EQ\n");col+=yyleng;}
"!="						{printf("NE\n");col+=yyleng;}
">"							{printf("GT\n");col+=yyleng;}
"<"							{printf("LT\n");col+=yyleng;}
">="						{printf("GE\n");col+=yyleng;}
"<="						{printf("LE\n");col+=yyleng;}
"+"							{printf("PLUS\n");col+=yyleng;}
"-"							{printf("MINUS\n");col+=yyleng;}
"/"							{printf("DIV\n");col+=yyleng;}
"*"							{printf("AST\n");col+=yyleng;}
"%"							{printf("MOD\n");col+=yyleng;}
"&"							{printf("AMP\n");col+=yyleng;}
"&&"						{printf("AND\n");col+=yyleng;}
"||"						{printf("OR\n");col+=yyleng;}
"!"							{printf("NOT\n");col+=yyleng;}
[ \t]						col+=yyleng;
\n							col=1;line++;
.							{printf("Line %d, col %d: illegal character ('%c')\n",line,col,yytext[0]);col+=yyleng;}


%%
int main(int argc, char *argv[])
{
	yylex();
	return 0;
}

int yywrap()
{
	return 1;
}
