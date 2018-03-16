/* Filename		: CNoyLexer.cpp
 * Description	: This contains the implementation of the CNoyLexer class and its methods
 * Authors		: Maureen Lauron
 * 				  Glenn Fabia
 * 				  Emmanuel Oppong Afriyie
 */
 
#include "CNoyLexer.h"

CNoyLexer::CNoyLexer(ifstream *fin) {
    this->sLine = 1;
    this->retract = true;
    this->composite = true;
    this->sourceF = fin;
    storeReservedWords();
}

void CNoyLexer::readChar() {
    if (sourceF->is_open()) {
        currChar = sourceF->get();
    }
}

char CNoyLexer::peek() {
    char p = sourceF->get();
    proxCC = p;
    return p;
}

bool CNoyLexer::scanning() {
    if (currChar == ' ' || currChar == '\t' || currChar == '\r') {
        return true;
    } else if (currChar == '\n') {
        sLine++;
        return true;
    } else if (currChar == '/') {
        char temp = peek();
        if (temp == '/') {
            temp = peek();
            while (temp != '\n') {
                temp = peek();
            }
            sLine++;
            return true;
        }
    }

    return false;
}

Token CNoyLexer::tokenizing() {
    Token retTokz;
    //tokenizing compositioned lexemes
    switch (currChar) {
        case '<':
            if (peek() == '=') {
                retTokz.lexeme = "<=";
                retTokz.tag = LEQ;
                retTokz.type = "relOp";
                return retTokz;
            } else {
                retract = false;
                composite = true;
                retTokz.lexeme = "<";
                retTokz.tag = LT;
                retTokz.type = "relOp";
                return retTokz;
            }
        case '>':
            if (peek() == '=') {
                retTokz.lexeme = ">=";
                retTokz.tag = GEQ;
                retTokz.type = "relOp";
                return retTokz;
            } else {
                retract = false;
                composite = true;
                retTokz.lexeme = ">";
                retTokz.tag = GT;
                retTokz.type = "relOp";
                return retTokz;
            }

        case '!':
            if (peek() == '=') {
                retTokz.lexeme = "!=";
                retTokz.tag = NEQ;
                retTokz.type = "relOp";
                return retTokz;
            } else {
                retract = false;
                composite = true;
                retTokz.lexeme = "!";
                retTokz.tag = NOT;
                retTokz.type = "booleanOp";
                return retTokz;
            }

        case '=':
            if (peek() == '=') {
                retTokz.lexeme = "==";
                retTokz.tag = EQ;
                retTokz.type = "relOp";
                return retTokz;
            } else {
                retract = false;
                composite = true;
                retTokz.lexeme = "=";
                retTokz.tag = ASSIGN;
                retTokz.type = "assOp";
                return retTokz;
            }

        case '&':
            if (peek() == '&') {
                retTokz.lexeme = "&&";
                retTokz.tag = AND;
                retTokz.type = "booleanOp";
                return retTokz;
            }

        case '|':
            if (peek() == '|') {
                retTokz.lexeme = "||";
                retTokz.tag = AND;
                retTokz.type = "booleanOp";
                return retTokz;
            }
    }
    //tokenizing integer & float literals
    //note that in the floating literals, I did not try to get the value in float since I need to clarify if do I really still need to
    if (isdigit(currChar)) {
        string intVal = "";
        do {
            intVal = intVal + currChar;
            readChar();
        } while (isdigit(currChar));

        if (currChar != '.') {
            retract = false;
            composite = false;
            retTokz.lexeme = intVal;
            retTokz.tag = INTLIT;
            retTokz.type = "numLit";
            return retTokz;
        }
        if (currChar == '.') {
            string floatVal = "";
            do {
                floatVal = floatVal + currChar;
                readChar();
            } while (isdigit(currChar));
            if (currChar != 'e') {
                retract = false;
                composite = false;
                floatVal = intVal + floatVal;
                retTokz.lexeme = floatVal;
                retTokz.tag = FLTLIT;
                retTokz.type = "numLit";
                return retTokz;
            }
        }
    }
        //tokenizing for the identifier part
    else if (isalpha(currChar)) {
        string ident;
        do {
            ident = ident + currChar;
            readChar();
        } while (isalpha(currChar) || isdigit(currChar));
        if (symbolTable.count(ident)) {
            retract = false;
            composite = false;
            retTokz.lexeme = ident;
            map<string, int>::iterator tempMap;
            for (tempMap = symbolTable.begin(); tempMap != symbolTable.end(); tempMap++) {
                if (tempMap->first == ident) {
                    retTokz.tag = tempMap->second;
                }
            }
            retTokz.type = "keyword";
            return retTokz;
        }
        symbolTable[ident] = IDNAME;

        retract = false;
        composite = false;
        retTokz.lexeme = ident;
        retTokz.tag = IDNAME;
        retTokz.type = "identifier";
        return retTokz;
    }
    else if (currChar == '"') {
        string strLit = "\"";
        do {
            readChar();
            if (currChar != '"')
                strLit = strLit + currChar;
        } while (currChar != '"' && currChar != -1);

        if (currChar == '"') {
            strLit += "\"";
        } else {
            cout << filename << ":  sa linya " << sLine << " :  walang panghuling \" simbolo.\n";
            errors++;
        }
        retTokz.lexeme = strLit;
        retTokz.tag = STRLIT;
        retTokz.type = "strLit";

        return retTokz;
    } else if (currChar == '+') {
        retTokz.lexeme = currChar;
        retTokz.tag = PLUS;
        retTokz.type = "arithOp";
        return retTokz;
    } else if (currChar == '-') {
        retTokz.lexeme = currChar;
        retTokz.tag = MINUS;
        retTokz.type = "arithOp";
        return retTokz;
    } else if (currChar == '*') {
        retTokz.lexeme = currChar;
        retTokz.tag = MUL;
        retTokz.type = "arithOp";
        return retTokz;
    } else if (currChar == '/') {
        retTokz.lexeme = currChar;
        retTokz.tag = DIV;
        retTokz.type = "arithOp";
        return retTokz;
    } else if (currChar == '%') {
        retTokz.lexeme = currChar;
        retTokz.tag = MOD;
        retTokz.type = "arithOp";
        return retTokz;
    } else if (currChar == '(') {
        retTokz.lexeme = currChar;
        retTokz.tag = OPENPAR;
        retTokz.type = "grpSym";
        return retTokz;
    } else if (currChar == ')') {
        retTokz.lexeme = currChar;
        retTokz.tag = CLOSEPAR;
        retTokz.type = "grpSym";
        return retTokz;
    } else if (currChar == '{') {
        retTokz.lexeme = currChar;
        retTokz.tag = OPENCURLY;
        retTokz.type = "grpSym";
        return retTokz;
    } else if (currChar == '}') {
        retTokz.lexeme = currChar;
        retTokz.tag = CLOSECURLY;
        retTokz.type = "grpSym";
        return retTokz;
    } else if (currChar == ',') {
        retTokz.lexeme = currChar;
        retTokz.tag = COMMA;
        retTokz.type = "grpSym";
        return retTokz;
    } else if (currChar == ';') {
        retTokz.lexeme = currChar;
        retTokz.tag = SEMICOLON;
        retTokz.type = "grpSym";
        return retTokz;
    } else if (currChar == '[') {
        retTokz.lexeme = currChar;
        retTokz.tag = OPENBRACE;
        retTokz.type = "arrSym";
        return retTokz;
    } else if (currChar == ']') {
        retTokz.lexeme = currChar;
        retTokz.tag = CLOSEBRACE;
        retTokz.type = "arrSym";
        return retTokz;
    } else if (currChar == -1) // NBSP or EOF
    {
        retTokz.lexeme = "EOF";
        retTokz.tag = 0;
        retTokz.type = "EOF";
    }
    else {
        cout << filename << ":  sa linya " << sLine << " :  may hindi wastong simbolo '" << currChar << "'.\n";
        errors++;
        retTokz.lexeme = currChar;
        retTokz.tag = -1;
        retTokz.type = "invalid symbol";
        return retTokz;
    }
}

Token CNoyLexer::getNextToken() {
    bool s = true;
    while (s) {
        if (retract)
            readChar();
        else {
            if (composite)
                currChar = proxCC;

        }
        retract = true;
        s = scanning();
    }
    token = tokenizing();
    return token;
}

void CNoyLexer::storeReservedWords() {
    symbolTable["kung"] = IF;
    symbolTable["kundi"] = ELSE;
    symbolTable["habang"] = WHILE;
    symbolTable["magtapos"] = RETURN;
    symbolTable["simula"] = MAIN;
    symbolTable["ipakita"] = PRINT;
    symbolTable["kumuha"] = READ;
    symbolTable["buo"] = INT;
    symbolTable["numero"] = FLOAT;
}

int CNoyLexer::getNextTokenID() {
    token = getNextToken();
    return token.tag;
}

int CNoyLexer::getLineNo() {
    return sLine;
}

string CNoyLexer::getLexeme() {
    return token.lexeme;
}
