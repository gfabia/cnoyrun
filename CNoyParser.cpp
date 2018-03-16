/* Filename		: CNoyLexer.cpp
 * Description	: This contains the implementation of the CNoyParser 
 *                (ie, functions for imposing grammar on the source code)
 *                as well as the necessary functions to generate the 
 *                intermediate codes (TAC and the virtual machine instructions)
 * 
 * Authors		: Glenn Fabia
 * 				  Maureen Lauron
 * 				  Emmanuel Oppong Afriyie
 */
 
#include "globals.h"
#include "CNoyLexer.h"
#include "CNoyCode.h"

// ---------------------------------------------------------------------
// Interpreter Declarations
// ---------------------------------------------------------------------

extern CNoyInterpreter vmach;

// ---------------------------------------------------------------------
// Lexical Analyzer Variable Declarations
// ---------------------------------------------------------------------

extern CNoyLexer *lexer;

// ---------------------------------------------------------------------
// Parser Variable Declarations
// ---------------------------------------------------------------------

string lexeme;
int lookahead;


// ---------------------------------------------------------------------
// TAC Generator Declarations
// ---------------------------------------------------------------------

stack <string> pStack;
vector <string> taCodes;
SymbolTable sTable;

int offset = 0;

int width(string);

string newTemp();
string newLabel();
string unQuote(string str);

void insertCode(string, string);
void insertCode(string, string, string);
void insertCode(string, string, string, string);
void insertCode(string, string, string, string, string);
void enter(string, string, int, string);
void checkVar(string);
void checkMatch(string, string, string);

bool isElementOf(int, int[]);
int scanTo(int [], int []);
void match(int);
void checkInput(int [], int []);

// Function to insert an identifier (variable) to the symbol table

void enter(string name, string type, int offst, string size) {
	int vsize = atoi(size.c_str());
	Symbol s(name, type, offst, vsize);

	if (sTable.insert(s) == false) {
		cout << filename << ":  sa linya " << lexer->getLineNo() << " :  labis na pagdeklara sa '" << name << "'" << endl;
		errors++;
	} else {
		char offstr[10];
		sprintf(offstr, "%d", offset);
		insertCode("enter", name, type, string(offstr), size);
		vmach.insert(&CNoyInterpreter::enter, name, type, size);
		offset = offset + vsize * width(type);
	}
}

// Returns the width (in bytes) needed for each data type.

int width(string type) {
	if (type == "buo")
		return 4; 
	else if (type == "numero")
		return 8;
	else if (type == "undefined")
		return 8;
}

// Function to create new temporary variables. Used in TAC generation.

string newTemp() {
	static int counter = 1;
	char tempStr[25];
	sprintf(tempStr, "$%d", counter++);
	enter(tempStr, "undefined", offset, "1");
	return string(tempStr);
}


// Function to create new code labels. Used in TAC generation.

string newLabel() {
	static int counter = 1;
	char tempStr[25];
	sprintf(tempStr, "@%d", counter++);
	return string(tempStr);
}


// Override function for inserting TAC. Used in TAC generation.
void insertCode(string op, string arg1) {
	insertCode(op, arg1, "", "", "");
}

// Override function for inserting TAC. Used in TAC generation.
void insertCode(string op, string arg1, string arg2) {
	insertCode(op, arg1, arg2, "", "");
}

// Override function for inserting TAC. Used in TAC generation.
void insertCode(string op, string arg1, string arg2, string arg3) {
	insertCode(op, arg1, arg2, arg3, "");
}

// Function to insert codes for each operation. Used in TAC generation.
void insertCode(string op, string arg1, string arg2, string arg3, string arg4) {
	string code;
	if (op == ":=") //arg2 := arg1
		code = (arg2 + " " + op + " " + arg1);
	else if (op == "uminus")
		code = (arg2 + " := -" + arg1);
	else if (op == "!")
		code = (arg2 + " := !" + arg1);
	
	else if (op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!="
		   || op == "+" || op == "-" || op == "||" || op == "*" || op == "/" || op == "%" || op == "&&")
		code = arg3 + " := " + arg1 + " " + op + " " + arg2;
	else if (op == "enter")
		code = ("enter " + arg1 + " " + arg2 + " " + arg3 + " " + (arg4 == "1" ? " " : arg4));
	else if (op == "++")
		code = (arg1 + " := " + arg1 + " + 1");
	else if (op == "goto")
		code = op + " " + arg1;
	else if (op == "iftrue")
		code = op + " " + arg1 + " goto " + arg2;
	else if (op == "label") {
		taCodes.push_back(arg1 + ": ");
		return;
	} else if (op == "param" || op == "return")
		code = op + " " + arg1;
	else if (op == "call")
		code = op + " " + arg1 + " " + arg2;
	else if (op == "halt")
		code = "halt";
	taCodes.push_back("   " + code);
}

// Function that checks if identifiers (variables) are declared or not. Used in semantic analysis.

void checkVar(string var) {
	if (sTable.search(var) == -1) {
		cout << filename << ":  sa linya " << lexer->getLineNo() << " :  hindi nakadeklarang identifier '" << var << "'" << endl;
		errors++;
	}
}

// Function that checks if operations has mathcing operand types or not. Used in semantic analysis.

void checkMatch(string op, string id1, string id2) {
	int a, b;
	if (sscanf(id1.c_str(), "$%d", &a) == 0 && sscanf(id2.c_str(), "$%d", &b) == 0) {
		if (sTable.search(id1) != -1 && sTable.search(id2) != -1) {
			if (sTable.getType(id1) == sTable.getType(id2)) {
				if (sTable.getSize(id1) != sTable.getSize(id2)) {
					cout << filename << ":  sa linya " << lexer->getLineNo() << " :  hindi wastong paraan sa paggamit ng '"
						   << op << "' sa " << id1 << endl;
					errors++;
				}
			} else {
				cout << filename << ":  sa linya " << lexer->getLineNo() << " :  hindi wastong paraan sa paggamit ng '"
					   << op << "' sa " << id1 << endl;
				errors++;
			}
		}
	}
}


// ---------------------------------------------------------------------
// Parser Utilities
// ---------------------------------------------------------------------

// Functio that removes the pair of quotes delimiting a string literal

string unQuote(string str) {
	return str.substr(1, str.length() - 2);
}

// Function to check for expected token

void match(int expectedToken) {
	lexeme = lexer->getLexeme();
	if (lookahead == expectedToken) {
		lookahead = lexer->getNextTokenID();
	} else {
		if (lookahead != 0)
		{
			cout << filename << ":  sa linya " << lexer->getLineNo() << " :  maling token na '"
				   << lexeme << "' ang nakita.\n";
			errors++;
		}
	}
}

bool isElementOf(int e, int set[]) {
	for (int i = 0; set[i] != -1; i++)
		if (e == set[i])
			return true;
	return false;
}

int scanTo(int set1[], int set2[]) {
	int tokensRead = 0;
	while (!((isElementOf(lookahead, set1)) || (isElementOf(lookahead, set2)) || (lookahead == 0))) {
		lookahead = lexer->getNextTokenID();
		tokensRead++;
	}
	return tokensRead;
}

void checkInput(int firstSet[], int followSet[]) {
	if (!isElementOf(lookahead, firstSet)) {
		string unexpected = (lookahead != STRLIT) ? lexer->getLexeme() : unQuote(lexer->getLexeme());
		int lineno = lexer->getLineNo();
		int skipped = scanTo(firstSet, followSet);
		if (skipped > 0) {
			cout << filename << ":  sa linya " << lineno << " :  maling token na '"
				   << unexpected << "' ang nakita, " << skipped << " mga token(s) ang nilaktawan.\n";
			errors++;
		}
	}
}

TreeNode *CNoyParse() {
	lookahead = lexer->getNextTokenID();
	return program();
}

/* program -> programHeading { declarations optionalStatements }
 *          | epsilon
 *
 * FIRST SET  : { INT }
 * FOLLOW SET : { EOF }
 */

TreeNode *program() {
	int first[] = {INT, -1};
	int follow[] = {0, -1};

	TreeNode *root = new TreeNode("program");

	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		
		root->addChild(programHeading());
		
		match('{');
		
		root->addChild(new TreeNode("{"));
		
		root->addChild(declarations());
		root->addChild(optionalStatements());
		
		match('}');
		
		root->addChild(new TreeNode("}"));

		insertCode("halt", "");

		vmach.insert(&CNoyInterpreter::halt, "", "", "");
		checkInput(follow, first);

	}
	return root;
}

/* programHeading -> int main ( )
 *
 * FIRST SET  : { INT }
 * FOLLOW SET : { '{' }
 */

TreeNode *programHeading() {
	int first[] = {INT, -1};
	int follow[] = {'{', -1};
	TreeNode *root = new TreeNode("programHeading");
	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		match(INT);
		root->addChild(new TreeNode("INT"));
		match(MAIN);
		root->addChild(new TreeNode("MAIN"));
		match('(');
		root->addChild(new TreeNode("("));
		match(')');
		root->addChild(new TreeNode(")"));
		checkInput(follow, first);
	}
	return root;
}

/* declarations -> [type identifierList ;]*
 *
 * FIRST SET  : { INT, FLOAT }
 * FOLLOW SET : { IDNAME, READ, PRINT, RETURN, '{', IF, WHILE }
 */

TreeNode *declarations() {
	int first[] = {INT, FLOAT, -1};
	int follow[] = {IDNAME, READ, PRINT, RETURN, '{', '}', IF, WHILE, -1};
	TreeNode *root = NULL;
	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		while (lookahead == INT || lookahead == FLOAT) {
			root = new TreeNode("declarations");
			root->addChild(type());
			root->addChild(identifierList());
			match(';');
			root->addChild(new TreeNode(";"));

			pStack.pop(); //pop type
		}
		checkInput(follow, first);
	}
	return root;
}

/* identifierList -> IDNAME size [, IDNAME size]*
 *
 * FIRST SET  : { IDNAME }
 * FOLLOW SET : { ';' }
 */

TreeNode *identifierList() {
	int first[] = {IDNAME, -1};
	int follow[] = {';', -1};

	TreeNode *root = new TreeNode("identifierList");
	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		string temp = "IDNAME '";
		temp += lexer->getLexeme();
		temp += "'";
		root->addChild(new TreeNode(temp));

		string vType = pStack.top();
		string vName = lexer->getLexeme();

		match(IDNAME);
		root->addChild(size());

		string vSize = pStack.top();
		pStack.pop();
		enter(vName, vType, offset, vSize);

		while (lookahead == ',') {
			match(',');
			root->addChild(new TreeNode(","));
			temp = "IDNAME '";
			temp += lexer->getLexeme();
			temp += "'";
			root->addChild(new TreeNode(temp));

			vName = lexer->getLexeme();

			match(IDNAME);

			vSize = pStack.top();

			root->addChild(size());

			vSize = pStack.top();
			pStack.pop();
			enter(vName, vType, offset, vSize);
		}
		checkInput(follow, first);
	}
	return root;
}

/* size -> [ INTLIT ]
 *       | epsilon
 *
 * FIRST SET  : { '[' }
 * FOLLOW SET : { ';', ',' }
 */

TreeNode *size() {
	int first[] = {'[', -1};
	int follow[] = {';', ',', -1};

	TreeNode *root = NULL;
	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		root = new TreeNode("size");
		match('[');
		root->addChild(new TreeNode("["));
		string temp = "INTLIT '";
		temp += lexer->getLexeme();
		temp += "'";
		root->addChild(new TreeNode(temp));

		pStack.push(lexer->getLexeme());

		match(INTLIT);
		match(']');
		root->addChild(new TreeNode("]"));
		checkInput(follow, first);
	} else {
		pStack.push("1");
	}

	return root;
}

/* type -> INT
 *       | FLOAT
 *
 * FIRST SET  : { INT, FLOAT }
 * FOLLOW SET : { IDNAME }
 */

TreeNode *type() {
	int first[] = {INT, FLOAT, -1};
	int follow[] = {IDNAME, -1};

	TreeNode *root = new TreeNode("type");
	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		if (lookahead == INT) {

			pStack.push(lexer->getLexeme());

			match(INT);
			root->addChild(new TreeNode("INT"));
		} else if (lookahead == FLOAT) {

			pStack.push(lexer->getLexeme());

			match(FLOAT);
			root->addChild(new TreeNode("FLOAT"));
		}
		checkInput(follow, first);
	}
	return root;
}

/* optionalStatements -> [statement]*
 *
 * FIRST SET  : { '{', IDNAME, READ, PRINT, RETURN, IF, WHILE }
 * FOLLOW SET : { '}' }
 */

TreeNode *optionalStatements() {
	int first[] = {'{', IDNAME, READ, PRINT, RETURN, IF, WHILE, -1};
	int follow[] = {'}', -1};

	TreeNode *root = NULL;
	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		root = new TreeNode("optionalStatements");
		while (isElementOf(lookahead, first)) {
			root->addChild(statement());
		}
		checkInput(follow, first);
	}
	return root;
}

/* statement -> assign
 *            | read ( variable ) ;
 *            | print ( toPrint ) ;
 *            | return expression0 ;
 *            | { optionalStatements }	
 *            | if ( expression ) statement elsePart
 *            | while ( expression ) statement
 
 * FIRST SET  : { '{', IDNAME, READ, PRINT, RETURN, IF, WHILE }
 * FOLLOW SET : { '}', '{', IDNAME, READ, PRINT, RETURN, IF, WHILE, ELSE }
 */

TreeNode *statement() {
	int first[] = {'{', IDNAME, READ, PRINT, RETURN, IF, WHILE, -1};
	int follow[] = {'}', '{', IDNAME, READ, PRINT, RETURN, IF, WHILE, ELSE, -1};

	TreeNode *root = NULL;
	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		root = new TreeNode("statement");
		string t;
		if (lookahead == IDNAME) {
			root->addChild(assign());
		} else if (lookahead == READ) {
			match(READ);
			root->addChild(new TreeNode("READ"));
			match('(');
			root->addChild(new TreeNode("("));
			root->addChild(variable());
			match(')');
			root->addChild(new TreeNode(")"));
			match(';');
			root->addChild(new TreeNode(";"));

			string t1;
			t1 = pStack.top();
			pStack.pop();
			insertCode("param", t1);
			vmach.insert(&CNoyInterpreter::param, t1, "", "");
			insertCode("call", "kumuha");
			vmach.insert(&CNoyInterpreter::call, "kumuha", "", "");

		} else if (lookahead == PRINT) {
			match(PRINT);
			root->addChild(new TreeNode("PRINT"));
			match('(');
			root->addChild(new TreeNode("("));
			root->addChild(toPrint());
			match(')');
			root->addChild(new TreeNode(")"));
			match(';');
			root->addChild(new TreeNode(";"));

			t = pStack.top();
			pStack.pop();
			insertCode("param", t);
			vmach.insert(&CNoyInterpreter::param, t, "", "");
			insertCode("call", "ipakita", "1", "");
			vmach.insert(&CNoyInterpreter::call, "ipakita", "1", "");

		} else if (lookahead == RETURN) {
			match(RETURN);
			root->addChild(new TreeNode("RETURN"));
			root->addChild(expression());

			t = pStack.top();
			pStack.pop();
			insertCode("magtapos", t);
			vmach.insert(&CNoyInterpreter::ret, t, "", "");
			match(';');
			root->addChild(new TreeNode(";"));
		} else if (lookahead == '{') {
			match('{');
			root->addChild(new TreeNode("{"));
			root->addChild(optionalStatements());
			match('}');
			root->addChild(new TreeNode("}"));
		} else if (lookahead == IF) {
			match(IF);
			root->addChild(new TreeNode("IF"));
			match('(');
			root->addChild(new TreeNode("("));
			root->addChild(expression());
			match(')');
			root->addChild(new TreeNode(")"));

			string l1 = newLabel();
			string l2 = newLabel();
			string l3 = newLabel();
			string t = pStack.top();
			pStack.pop();

			insertCode("iftrue", t, l1, "");
			vmach.insert(&CNoyInterpreter::iftrue, t, l1, "");

			insertCode("goto", l2);
			vmach.insert(&CNoyInterpreter::go, l2, "", "");

			insertCode("label", l1);
			vmach.setLabel(l1, vmach.getSize());

			root->addChild(statement());

			insertCode("goto", l3);
			vmach.insert(&CNoyInterpreter::go, l3, "", "");

			insertCode("label", l2);
			vmach.setLabel(l2, vmach.getSize());

			root->addChild(elsePart());

			insertCode("label", l3);
			vmach.setLabel(l3, vmach.getSize());

		} else if (lookahead == WHILE) {

			string l1 = newLabel();
			string l2 = newLabel();
			string l3 = newLabel();
			insertCode("label", l1);
			vmach.setLabel(l1, vmach.getSize());

			match(WHILE);
			root->addChild(new TreeNode("WHILE"));
			match('(');
			root->addChild(new TreeNode("("));

			root->addChild(expression());

			string t = pStack.top();
			pStack.pop();
			insertCode("iftrue", t, l2, "");
			vmach.insert(&CNoyInterpreter::iftrue, t, l2, "");

			insertCode("goto", l3);
			vmach.insert(&CNoyInterpreter::go, l3, "", "");

			match(')');
			root->addChild(new TreeNode(")"));

			insertCode("label", l2);
			vmach.setLabel(l2, vmach.getSize());

			root->addChild(statement());

			insertCode("goto", l1);
			vmach.insert(&CNoyInterpreter::go, l1, "", "");

			insertCode("label", l3);
			vmach.setLabel(l3, vmach.getSize());
		}
		checkInput(follow, first);
	}
	return root;
}

/* assign -> variable = expression ;
 * 
 * FIRST SET  : { IDNAME }
 * FOLLOW SET : { '}', IDNAME, READ, PRINT, RETURN, '{', IF, WHILE, ELSE}
 */

TreeNode *assign() {
	int first[] = {IDNAME, -1};
	int follow[] = {'}', '{', IDNAME, READ, PRINT, RETURN, IF, WHILE, ELSE, -1};
	TreeNode *root = new TreeNode("assign");

	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		root->addChild(variable());
		string a, b, t;
		if (lookahead == '=') {
			match('=');
			root->addChild(new TreeNode("="));
			root->addChild(expression());

			b = pStack.top();
			pStack.pop();
			a = pStack.top();
			pStack.pop();
			checkMatch("=", a, b);
			insertCode(":=", b, a);

			vmach.insert(&CNoyInterpreter::assign, b, a, "");

			match(';');
			root->addChild(new TreeNode(";"));
		}
		checkInput(follow, first);
	}
	return root;
}

/* toPrint -> STRLIT
 *          | expression
 *
 * FIRST SET  : { STRLIT, IDNAME, INTLIT, FLTLIT, '(', '!', '-', '+' }
 * FOLLOW SET : { ')' }
 */

TreeNode *toPrint() {
	int first[] = {STRLIT, IDNAME, INTLIT, FLTLIT, '(', '!', '-', '+', -1};
	int follow[] = {')', -1};
	TreeNode *root = new TreeNode("toPrint");

	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		if (lookahead == STRLIT) {
			string temp = "STRLIT '";
			temp += unQuote(lexer->getLexeme());
			temp += "'";
			root->addChild(new TreeNode(temp));

			pStack.push(lexer->getLexeme());

			match(STRLIT);
		} else
			root->addChild(expression());
		checkInput(follow, first);
	}
	return root;
}

/* elsePart -> else statement
 *           | epsilon
 *
 * FIRST SET  : { ELSE }
 * FOLLOW SET : { '}', ELSE, IDNAME, READ, PRINT, RETURN, '{', IF, WHILE }
 */

TreeNode *elsePart() {
	int first[] = {ELSE, -1};
	int follow[] = {'}', ELSE, IDNAME, READ, PRINT, RETURN, '{', IF, WHILE, -1};

	TreeNode *root = NULL;
	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		if (lookahead == ELSE) {
			root = new TreeNode("elsePart");
			match(ELSE);
			root->addChild(new TreeNode("ELSE"));
			root->addChild(statement());
		}
		checkInput(follow, first);
	}
	return root;
}

/* variable -> IDNAME location
 *           | epsilon
 *
 * FIRST SET  : { IDNAME }
 * FOLLOW SET : { '=', ')' }
 */

TreeNode *variable() {
	int first[] = {IDNAME, -1};
	int follow[] = {'=', ')', -1};

	TreeNode *root = new TreeNode("variable");
	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		string temp = "IDNAME '";
		temp += lexer->getLexeme();
		temp += "'";
		root->addChild(new TreeNode(temp));

		string var = lexer->getLexeme();
		pStack.push(var);
		checkVar(var);

		match(IDNAME);
		root->addChild(location());

		checkInput(follow, first);
	}
	return root;
}

/* location -> [ expression ]
 *
 * FIRST SET  : { '[' }
 * FOLLOW SET : { '=', ')' }
 */

TreeNode* location() {
	int first[] = {'[', -1};
	int follow[] = {'=', ')', -1};

	TreeNode *root = NULL;
	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		if (lookahead == '[') {
			root = new TreeNode("location");
			match('[');
			root->addChild(new TreeNode("["));
			root->addChild(expression());
			match(']');
			root->addChild(new TreeNode("]"));

			string t1, t2;
			t2 = pStack.top();
			pStack.pop();
			t1 = pStack.top();
			pStack.pop();

			pStack.push(t1 + "[" + t2 + "]");
		}
		checkInput(follow, first);
	}
	return root;
}

/* expressionList -> expression [, expression]*
 *
 * FIRST SET  : { '(', INTLIT, IDNAME, FLTLIT, '!', '-', '+' }
 * FOLLOW SET : { ')' }
 */

TreeNode *expressionList() {
	int first[] = {'(', INTLIT, IDNAME, FLTLIT, '!', '-', '+', -1};
	int follow[] = {')', -1};

	TreeNode *root = new TreeNode("expressionList");

	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		root->addChild(expression());
		while (lookahead == ',') {
			match(',');
			root->addChild(new TreeNode(","));
			root->addChild(expression());

		}
		checkInput(follow, first);
	}

	return root;
}

/* expression -> simpleExpression 
 *             | simpleExpression > simpleExpression
 *             | simpleExpression == simpleExpression
 *             | simpleExpression != simpleExpression
 *             | simpleExpression <= simpleExpression
 *             | simpleExpression >= simpleExpression
 *             | epsilon
 *
 * FIRST SET  : { '(', INTLIT, IDNAME, FLTLIT, '!', '-', '+' }
 * FOLLOW SET : { ')', ']', ',', ';', EQ, GEQ, LEQ, NEQ, '<', '>' }
			 { ')', ';', ']', ',', '?', ':' }
 */

TreeNode *expression() {
	int first[] = {'(', INTLIT, IDNAME, FLTLIT, '!', '-', '+', -1};
	int follow[] = {')', ';', ']', ',', '?', ':', -1};


	TreeNode *root = new TreeNode("expression");
	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		string se1, se2, t;
		root->addChild(simpleExpression());
		if (lookahead == '<') {
			match('<');
			root->addChild(new TreeNode("<"));
			root->addChild(simpleExpression());

			se2 = pStack.top();
			pStack.pop();
			se1 = pStack.top();
			pStack.pop();
			t = newTemp();
			checkMatch("<", se1, se2);
			insertCode("<", se1, se2, t);
			vmach.insert(&CNoyInterpreter::lt, se1, se2, t);
			pStack.push(t);
		} else if (lookahead == '>') {
			match('>');
			root->addChild(new TreeNode(">"));
			root->addChild(simpleExpression());

			se2 = pStack.top();
			pStack.pop();
			se1 = pStack.top();
			pStack.pop();
			t = newTemp();
			checkMatch(">", se1, se2);
			insertCode(">", se1, se2, t);
			vmach.insert(&CNoyInterpreter::gt, se1, se2, t);
			pStack.push(t);
		} else if (lookahead == EQ) {
			match(EQ);
			root->addChild(new TreeNode("=="));
			root->addChild(simpleExpression());

			se2 = pStack.top();
			pStack.pop();
			se1 = pStack.top();
			pStack.pop();
			t = newTemp();
			checkMatch("==", se1, se2);
			insertCode("==", se1, se2, t);
			vmach.insert(&CNoyInterpreter::eq, se1, se2, t);
			pStack.push(t);
		} else if (lookahead == LEQ) {
			match(LEQ);
			root->addChild(new TreeNode("<="));
			root->addChild(simpleExpression());

			se2 = pStack.top();
			pStack.pop();
			se1 = pStack.top();
			pStack.pop();
			t = newTemp();
			checkMatch("<=", se1, se2);
			insertCode("<=", se1, se2, t);
			vmach.insert(&CNoyInterpreter::leq, se1, se2, t);
			pStack.push(t);
		} else if (lookahead == GEQ) {
			match(GEQ);
			root->addChild(new TreeNode(">="));
			root->addChild(simpleExpression());

			se2 = pStack.top();
			pStack.pop();
			se1 = pStack.top();
			pStack.pop();
			t = newTemp();
			checkMatch(">=", se1, se2);
			insertCode(">=", se1, se2, t);

			vmach.insert(&CNoyInterpreter::geq, se1, se2, t);
			pStack.push(t);
		} else if (lookahead == NEQ) {
			match(NEQ);
			root->addChild(new TreeNode("!="));
			root->addChild(simpleExpression());

			se2 = pStack.top();
			pStack.pop();
			se1 = pStack.top();
			pStack.pop();
			t = newTemp();
			checkMatch("!=", se1, se2);
			insertCode("!=", se1, se2, t);

			vmach.insert(&CNoyInterpreter::neq, se1, se2, t);
			pStack.push(t);
		}
		checkInput(follow, first);
	}
	return root;
}

/* simpleExpression -> term [addOp term]*
 *
 * FIRST SET  : { '(', INTLIT, IDNAME, FLTLIT, '!', '-', '+' }
 * FOLLOW SET : { ')', ';', ']', ',', '?', ':', '<', '>', EQ, LEQ, GEQ, NEQ }
 */

TreeNode *simpleExpression() {
	int first[] = {'(', IDNAME, INTLIT, FLTLIT, '!', '-', '+', -1};
	int follow[] = {')', ';', ']', ',', '?', ':', '<', '>', EQ, LEQ, GEQ, NEQ, -1};

	TreeNode *root = new TreeNode("simpleExpression");

	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		root->addChild(term());
		while (lookahead == OR || lookahead == '+' || lookahead == '-') {
			root->addChild(addOp());
			root->addChild(term());

			string a, b, t, op;
			b = pStack.top();
			pStack.pop();
			op = pStack.top();
			pStack.pop();
			a = pStack.top();
			pStack.pop();
			t = newTemp();
			checkMatch(op, a, b);
			insertCode(op, a, b, t);
			if (op == "+")
			vmach.insert(&CNoyInterpreter::add, a, b, t);
			else if (op == "-")
			vmach.insert(&CNoyInterpreter::sub, a, b, t);
			else
			vmach.insert(&CNoyInterpreter::lor, a, b, t);
			pStack.push(t);
		}
		checkInput(follow, first);
	}

	return root;
}

/* addOp -> +
 *        | -
 *        | ||
 *           
 * FIRST SET  : { '+', '-', OR }
 * FOLLOW SET : { '+', '-', INTLIT, IDNAME, FLTLIT, '(', '!' }
 */

TreeNode *addOp() {
	int first[] = {'+', '-', OR, -1};
	int follow[] = {'+', '-', INTLIT, IDNAME, FLTLIT, '(', '!', -1};
	TreeNode *root = new TreeNode("addOp");
	checkInput(first, follow);
	if (lookahead == '+') {
		pStack.push("+");

		match('+');
		root->addChild(new TreeNode("+"));
	} else if (lookahead == '-') {
		pStack.push("-");

		match('-');
		root->addChild(new TreeNode("-"));
	} else if (lookahead == OR) {
		pStack.push("||");

		match(OR);
		root->addChild(new TreeNode("||"));
	}
	checkInput(follow, first);
	return root;
}

/* term -> factor [mulOp factor]*
 *           
 * FIRST SET  : { '+', '-', INTLIT, IDNAME, FLTLIT, '(', '!' }
 * FOLLOW SET : { '+', '-', OR, ')', ';', ']', ',', '?', ':', '<', '>', EQ, LEQ, GEQ, NEQ }
 */

TreeNode *term() {
	int first[] = {'(', IDNAME, INTLIT, FLTLIT, '!', '-', '+', -1};
	int follow[] = {'+', '-', OR, ')', ';', ']', ',', '?', ':', '<', '>', EQ, LEQ, GEQ, NEQ, -1};

	TreeNode *root = new TreeNode("term");

	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		root->addChild(factor());
		while (lookahead == '/' || lookahead == '*' || lookahead == '%' || lookahead == AND) {
			root->addChild(mulOp());
			root->addChild(factor());

			string a, b, t, op;
			b = pStack.top();
			pStack.pop();
			op = pStack.top();
			pStack.pop();
			a = pStack.top();
			pStack.pop();
			t = newTemp();

			checkMatch(op, a, b);
			insertCode(op, a, b, t);
			if (op == "/")
			vmach.insert(&CNoyInterpreter::div, a, b, t);
			else if (op == "*")
			vmach.insert(&CNoyInterpreter::mul, a, b, t);
			else if (op == "%")
			vmach.insert(&CNoyInterpreter::mod, a, b, t);
			else
			vmach.insert(&CNoyInterpreter::land, a, b, t);
			pStack.push(t);
		}
		checkInput(follow, first);
	}

	return root;
}

/* mulOp -> *
 *        | /
 *        | %
 *        | &&
 *           
 * FIRST SET  : { '/', '*', '%', AND }
 * FOLLOW SET : { IDNAME, INTLIT, FLTLIT, '(', '!', '-', '+' }
 */

TreeNode *mulOp() {
	int first[] = {'/', '*', '%', AND, -1};
	int follow[] = {IDNAME, INTLIT, FLTLIT, '(', '!', '-', '+', -1};
	TreeNode *root = new TreeNode("mulOp");
	checkInput(first, follow);
	if (lookahead == '/') {
		pStack.push("/");

		match('/');
		root->addChild(new TreeNode("/"));
	} else if (lookahead == '*') {
		pStack.push("*");

		match('*');
		root->addChild(new TreeNode("*"));
	} else if (lookahead == '%') {
		pStack.push("%");

		match('%');
		root->addChild(new TreeNode("%"));
	} else if (lookahead == AND) {
		pStack.push("&&");

		match(AND);
		root->addChild(new TreeNode("&&"));
	}
	checkInput(follow, first);
	return root;
}

/* factor -> IDNAME modifier
 *         | INTLIT
 *         | FLTLIT
 *         | ( expression )
 *         | ! factor
 *         | - factor
 *         | + factor
 *           
 * FIRST SET  : { '+', '-', INTLIT, IDNAME, FLTLIT, '(', '!' }
 * FOLLOW SET : { '*', '/', '%', AND, '+', '-', OR, ')', ';', ']', ',',  '<', '>', EQ, LEQ, GEQ, NEQ }
 */

TreeNode *factor() {
	int first[] = {'(', IDNAME, INTLIT, FLTLIT, '!', '-', '+', -1};
	int follow[] = {'*', '/', '%', AND, '+', '-', OR, ')', ';', ']', ',', '<', '>', EQ, LEQ, GEQ, NEQ, -1};
	TreeNode *root = new TreeNode("factor");
	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		if (lookahead == '(') {
			match('(');

			root->addChild(new TreeNode("("));
			root->addChild(expression());

			match(')');
			root->addChild(new TreeNode(")"));

		} else if (lookahead == IDNAME) {
			string var = lexer->getLexeme();
			checkVar(var);
			pStack.push(var);

			string temp = "IDNAME '";
			temp += lexer->getLexeme();
			temp += "'";
			root->addChild(new TreeNode(temp));

			match(IDNAME);
			root->addChild(modifier());

		} else if (lookahead == INTLIT) {
			pStack.push(lexer->getLexeme());

			string temp = "INTLIT '";
			temp += lexer->getLexeme();
			temp += "'";
			root->addChild(new TreeNode(temp));
			match(INTLIT);
		} else if (lookahead == FLTLIT) {
			pStack.push(lexer->getLexeme());

			string temp = "FLTLIT '";
			temp += lexer->getLexeme();
			temp += "'";
			root->addChild(new TreeNode(temp));
			match(FLTLIT);
		} else if (lookahead == '+') {
			match('+');
			root->addChild(new TreeNode("+"));
			root->addChild(factor());
		} else if (lookahead == '-') {
			match('-');
			root->addChild(new TreeNode("-"));
			root->addChild(factor());

			string t1 = pStack.top();
			pStack.pop();
			pStack.push("-" + t1);

		} else if (lookahead == '!') {
			match('!');
			root->addChild(new TreeNode("!"));
			root->addChild(factor());

			string t3 = pStack.top();
			pStack.pop();
			string t4 = newTemp();
			insertCode("!", t3, t4);

			vmach.insert(&CNoyInterpreter::unot, t3, t4, "");
			pStack.push(t4);
		}
		checkInput(follow, first);
	}
	return root;
}

/* modifier -> [ expression ] 
 *           | epsilon
 *           
 * FIRST SET  : { '[' }
 * FOLLOW SET : { '*', '/', '%', AND, '+', '-', OR, ')', ';', ']', ',',  '<', '>', EQ, LEQ, GEQ, NEQ }
 */

TreeNode* modifier() {
	int first[] = {'[', -1};
	int follow[] = {'*', '/', '%', AND, '+', '-', OR, ')', ';', ']', ',', '<', '>', EQ, LEQ, GEQ, NEQ, -1};

	TreeNode *root = new TreeNode("modifier");
	checkInput(first, follow);
	if (isElementOf(lookahead, first)) {
		if (lookahead == '[') {
			match('[');
			root->addChild(new TreeNode("["));
			root->addChild(expression());

			string t1, t2, t3;
			t3 = newTemp();
			t2 = pStack.top();

			pStack.pop();

			t1 = pStack.top();
			pStack.pop();

			insertCode(":=", t1 + "[" + t2 + "]", t3);
			checkMatch("[]", t1 + "[" + t2 + "]", t3);
			vmach.insert(&CNoyInterpreter::assign, t1 + "[" + t2 + "]", t3, "");

			match(']');
			root->addChild(new TreeNode("]"));

			pStack.push(t3);
		}

		checkInput(follow, first);
	}
	return root;
}
