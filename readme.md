Compilers Project
===================================
qC Compiler
-------------

#### Features: ####
* Lexical analyser
* Error handling
* Syntax Analyzer (Abstract Syntax Tree)
* Semantic Analyzer (Symbol Table + Error Handler)

*Installation:*

./install   (lex,yacc and gcc required)

####Usage:####

Show AST
	./qcompiler -t < simple.qc

Show Symbol Table
	./qcompiler -s < simple.qc

####Screenshot:####
<p align="center">
  <img src="https://github.com/AlexPnt/qC/blob/master/screens/qC.png"/>
</p>