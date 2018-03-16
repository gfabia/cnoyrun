# Filename		: Makefile
# Description	: Set up the build (compile) commands for CNoy Intepreter
# Authors		: Glenn Fabia
#                 Maureen Lauron
#                 Emmanuel Oppong Afriyie
	
cnoyrun:  main.o parser.o lexer.o interpreter.o
	g++ -o cnoyrun main.o interpreter.o parser.o lexer.o 
lexer.o	: CNoyLexer.h CNoyLexer.cpp
	g++ -c CNoyLexer.cpp -o lexer.o
parser.o : CNoyParser.cpp
	g++ -c CNoyParser.cpp -o parser.o
interpreter.o: CNoyCode.h CNoyCode.cpp
	g++ -c CNoyCode.cpp -o interpreter.o
main.o	: main.cpp
	g++ -c main.cpp -o main.o
clean	: 
	rm -rf *.o cnoyrun 
