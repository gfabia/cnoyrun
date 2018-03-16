/* Filename		: CNoyLexer.h
 * Description	: This contains the interface of the CNoyLexer class and its methods
 * Authors		: Maureen Lauron
 * 				  Glenn Fabia
 * 				  Emmanuel Oppong Afriyie
 */
 
#ifndef CNOYLEXER_H
#define CNOYLEXER_H

#include "globals.h"

using namespace std;

class Token {
public:
    int tag;
    string lexeme;
    string type;
    int line;
};

extern string filename;	// Source filename, declared in main.cpp

class CNoyLexer {
private:
    char currChar;		// Holds the current character read
    int sLine;			// Line number
    bool retract;		// Flag set when scanner needs to go back when scanning
    bool composite;		// Flag set when a composite token is met    
    char retTract;  	// Temporary variable to hold the retracted symbol
    char proxCC;		// Another temporary variable
    ifstream *sourceF;  // Source filename
    Token token; 		// Current token
    map <string, int> symbolTable; // Symbol table

public:
    CNoyLexer(ifstream *fin); // Constructor

    void readChar();  		// Reads a character from the source input
    char peek();      		// Function that returns the next character in the source input
    bool scanning(); 		// Scans for whitespaces and comments
    Token tokenizing(); 		// Main scanner function 
    void storeReservedWords(); 	// Places the reserved words in the symbol table

    string getLexeme();  	// Returns the lexeme machine the current Token
    Token getNextToken(); 	// Scans and returns the next Token
    int getNextTokenID(); 	// Scans and returns the next Token's ID
    int getLineNo();		// Returns the line number from where the current Token was seen
};

#endif /* _LEXER_H */

