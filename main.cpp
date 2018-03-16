/* Filename		: main.cpp
 * Description	: This is the driver program for the CNoy Interpreter
 * Authors		: Glenn Fabia
 * 				  Maureen Lauron
 * 				  Emmanuel Oppong Afriyie
 */
 
#include "globals.h"
#include "CNoyLexer.h"
#include "CNoyCode.h"

string filename; 	// Filename of the source code
int errors = 0;		// Error counter

CNoyLexer *lexer; 	// Lexical Analyzer
TreeNode *parseTree;// Root of Syntax Tree
extern vector <string> taCodes;	// Array of Three-Address Codes
CNoyInterpreter vmach; // Interpreter

// Function to display the syntax (parse) tree of the source language
void display(TreeNode *root, int indent, ostream &out) {
	if (root != NULL) {
		for (int i = 0; i < indent; i++)
			out << " | ";
		out << root->getName() << endl;
		for (int i = 0; i < root->numChildren(); i++)
			display(root->getChild(i), indent + 1, out);
	}
}

// Function to display the Three-Address Codes
void display(vector<string> codes, ostream &out) {
	int codeSize = codes.size();
	for (int i = 0; i < codeSize; i++) {
		out << codes[i] << endl;
	}
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "mali : walang input file." << endl;
		cout << "wastong paggamit : cnoyrun <file.cnoy>" << endl;
		return -1;
	}

	filename = argv[1];

	ifstream fin(filename.c_str());
	if (!fin) {
		cout << "mali : di mabuksan ang file '" << filename << "'." << endl;
		return -1;
	}

	lexer = new CNoyLexer(&fin);
	parseTree = CNoyParse();
	
	if (errors == 0) {
		// Create the intermediate representations namely:
		
		// [1] Syntax Tree
		string outParseTreeName = string(filename) + string(".tree");
		ofstream fOutPTree(outParseTreeName.c_str());
		if (fOutPTree)
			display(parseTree, 0, fOutPTree);
		fOutPTree.close();

		// [2] TAC 
		string outTACName = string(filename) + string(".tac");
		ofstream fOutTAC(outTACName.c_str());
		if (fOutTAC)
			display(taCodes, fOutTAC);
		fOutTAC.close();
		
		// Interpret codes
		vmach.run();
	} else
		cout << "\n" << errors << " na mga mali ang nakita." << endl;
	return 0;
}
