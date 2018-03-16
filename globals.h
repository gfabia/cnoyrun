/* Filename		: global.h
 * Description	: This contains the global include directives,
 *                token IDs definitions, and the abstractions for the 
 *                syntax tree nodes and symbol table
 * 
 * Authors		: Glenn Fabia
 * 				  Maureen Lauron
 * 				  Emmanuel Oppong Afriyie
 */

#ifndef _GLOBALS_H
#define	_GLOBALS_H

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdio>

#include <cstring>
#include <cctype>
#include <cstdlib>

#include <map>
#include <vector>
#include <stack>

using namespace std;


#define IF 202
#define ELSE 203
#define WHILE 204
#define RETURN 205
#define MAIN 400
#define READ 401
#define PRINT 402
#define INT 200
#define FLOAT 201
#define IDNAME 500
#define LT '<'
#define GT '>'
#define LEQ 601
#define GEQ 621
#define EQ  611
#define NEQ 633
#define NOT '!'
#define OR 248
#define AND 238
#define STRLIT 501
#define INTLIT 502
#define FLTLIT 503
#define PLUS '+'
#define MINUS '-'
#define MUL '*'
#define DIV '/'
#define MOD '%'
#define OPENPAR '('
#define CLOSEPAR ')'
#define OPENCURLY '{'
#define CLOSECURLY '}'
#define OPENBRACE '['
#define CLOSEBRACE ']'
#define ASSIGN '='
#define COMMA ','
#define SEMICOLON ';'

extern int errors;
extern vector <string> taCodes;


// Abstraction for a node in the Syntax Tree

#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
using namespace std;

class TreeNode {
	string name;
	vector <TreeNode *> child;

public:

	TreeNode(string n) {
		name = n;
	}

	void addChild(TreeNode *c) {
		child.push_back(c);
	}

	int numChildren() {
		return child.size();
	}

	string getName() {
		return name;
	}

	TreeNode *getChild(int i) {
		if (i < numChildren())
			return child[i];
		else
			return NULL;
	}
};

#endif // TREE_H

// Global declarations of the parser functions

TreeNode* CNoyParse();
TreeNode* program();
TreeNode* programHeading();
TreeNode* declarations();
TreeNode* identifierList();
TreeNode* size();
TreeNode* type();
TreeNode* optionalStatements();
TreeNode* statement();
TreeNode* assign();
TreeNode* toPrint();
TreeNode* elsePart();
TreeNode* variable();
TreeNode* location();
TreeNode* expressionList();
TreeNode* simpleExpression();
TreeNode* expression();
TreeNode* term();
TreeNode* addOp();
TreeNode* factor();
TreeNode* mulOp();
TreeNode* modifier();
void match(int expectedToken);

// Abstraction for the Symbol Table
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

struct Symbol {
	string name;	// Name of identifier
	string type;	// Data type
	int offset;		// Location index (as in a contiguous memory)
	vector<float> value;	// Value
	int size;		// Size

	Symbol(string n, string t, int o, int s) {
		name = n;
		type = t;
		offset = o;
		size = s;
		for (int i = 0; i < size; i++)
			value.push_back(0.0);
	}
};

struct SymbolTable {
	vector <Symbol> db;

	bool isArray(string id) {
		for (int i = 0; i < id.size(); i++) {
			if (id[i] == '[')
				return true;
		}
		return false;
	}

	string getArrayIndex(string arrID) {
		string temp;
		int i = 0;
		for (; i < arrID.size(); i++) {
			if (arrID[i] == '[')
				break;
		}
		for (int j = i + 1; j < arrID.size(); j++) {
			if (arrID[j] == ']')
				break;
			else
				temp = temp + arrID[j];
		}
		return temp;
	}

	string getArrayName(string arrID) {
		string temp;
		for (int i = 0; i < arrID.size(); i++) {
			if (arrID[i] == '[')
				break;
			temp = temp + arrID[i];
		}
		return temp;
	}

	bool insert(Symbol s) {
		if (search(s.name) == -1) {
			db.push_back(s);
			return true;
		}
		else
			return false;
	}

	int search(string n) {
		for (int i = 0; i < db.size(); i++)
			if (db[i].name == n)
				return i;
		return -1;
	}

	bool updateType(string n, string t) {
		int i = search(n);
		if (i != -1) {
			db[i].type = t;
			return true;
		}
		return false;
	}

	string getType(string n) {
		if (isArray(n)) {
			n = getArrayName(n);
		}

		int index = search(n);
		return (index != -1 ? db[index].type : "undefined");
	}

	int getSize(string n) {

		if (isArray(n)) {
			n = getArrayName(n);
		}

		int index = search(n);
		return (index != -1 ? db[index].size : -1);
	}

	void setValue(string n, float newValue) {
		int arr_indx = 0;

		if (isArray(n)) {
			string indx = getArrayIndex(n);
			n = getArrayName(n);
			if (isdigit(indx[0]))
				arr_indx = atoi(indx.c_str());
			else {
				int ix = search(indx);
				if (ix != -1) {
					arr_indx = (int) db[ix].value[0];
				}
			}
		}

		int i = search(n);
		if (i != -1) {
			if (arr_indx < 0 || arr_indx >= db[i].size) {
				cout << "exception caught: index out of bounds" << endl;
				exit(-1);
			} else {
				db[i].value[arr_indx] = newValue;
			}
		}
	}

	float getValue(string n) {

		int arr_indx = 0;

		if (isArray(n)) {
			string indx = getArrayIndex(n);
			n = getArrayName(n);
			if (isdigit(indx[0]))
				arr_indx = atoi(indx.c_str());
			else {
				int ix = search(indx);
				if (ix != -1) {
					arr_indx = (int) db[ix].value[0];
				}
			}
		}

		int i = search(n);
		if (i != -1) {
			if (arr_indx < 0 || arr_indx >= db[i].size) {
				cout << "exception caught: index out of bounds" << endl;
				exit(-1);
			} else {
				return db[i].value[arr_indx];
			}
		} else { //assume parameter as float literal
			return atof(n.c_str());
		}

		return -1;
	}

	void enter(string id, string ty, int offset) {
		Symbol s(id, ty, offset, 1);
		insert(s);
	}

	void enter(string id, string ty, int offset, int size) {
		Symbol s(id, ty, offset, size);
		insert(s);
	}
};

#endif	/* SYMBOLTABLE_H */


#endif	/* _GLOBALS_H */

