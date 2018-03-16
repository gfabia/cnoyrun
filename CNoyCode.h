/* Filename		: CNoyCode.h
 * Description	: This contains the interface for the CNoyInterpreter class
 *                This class makes possible the interpretation of the source input
 *                through a virtual machine representing the segments of a program
 *                (data, code, and stack).
 * 
 * Authors		: Glenn Fabia
 * 				  Maureen Lauron
 * 				  Emmanuel Oppong Afriyie
 */
 
#ifndef CNOYCODE_H
#define CNOYCODE_H

#include "globals.h"

class CNoyInterpreter {

	// Abstraction for a Three-Address Code
	struct tac {
		void (CNoyInterpreter::*op)(string, string, string);
		string arg1, arg2, arg3;

		tac(void (CNoyInterpreter::*o)(string, string, string), string a, string b, string c) {
			op = o;		// Operator
			arg1 = a;   // Operand 1
			arg2 = b;   // Operand 2
			arg3 = c;   // Operand 3
		}
	};
	
	SymbolTable sTable; 		// Data area
	vector <tac> code; 			// Code area
	stack <string> cStack; 		// Stack area
	int ip; 					// Instruction pointer (index)
	map <string, int> label; 	// Map of labels to index values
public:

	CNoyInterpreter();
	void insert(void (CNoyInterpreter::*o)(string, string, string),
		   string a, string b, string c);
	int getSize();
	void enter(string id, string dataType, string size);
	void assign(string arg1, string arg2, string arg3);
	void unot(string arg1, string arg2, string arg3);
	void add(string arg1, string arg2, string arg3);
	void sub(string arg1, string arg2, string arg3);
	void lor(string arg1, string arg2, string arg3);
	void div(string arg1, string arg2, string arg3);
	void mul(string arg1, string arg2, string arg3);
	void mod(string arg1, string arg2, string arg3);
	void land(string arg1, string arg2, string arg3);
	void leq(string arg1, string arg2, string arg3);
	void geq(string arg1, string arg2, string arg3);
	void eq(string arg1, string arg2, string arg3);
	void neq(string arg1, string arg2, string arg3);
	void lt(string arg1, string arg2, string arg3);
	void gt(string arg1, string arg2, string arg3);
	void iftrue(string arg1, string arg2, string arg3);
	void go(string arg1, string arg2, string arg3);
	void param(string arg1, string arg2, string arg3);
	void call(string arg1, string arg2, string arg3);
	void halt(string arg1, string arg2, string arg3);
	void ret(string arg1, string arg2, string arg3);
	void run();
	void setLabel(string name, int i);
};

#endif // CNOYCODE_H
