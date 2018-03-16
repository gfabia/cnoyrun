/* Filename		: CNoyCode.cpp
 * Description	: This contains the implementation of the CNoyInterpreter
 * 
 * Authors		: Glenn Fabia
 * 				  Maureen Lauron
 * 				  Emmanuel Oppong Afriyie
 */
 
 #include "CNoyCode.h"

CNoyInterpreter::CNoyInterpreter() {
	ip = 0;
}

void CNoyInterpreter::insert(void (CNoyInterpreter::*o)(string, string, string),
	   string a, string b, string c) {
	tac x(o, a, b, c);
	code.push_back(x);
}

int CNoyInterpreter::getSize() {
	return code.size();
}

// enter:	 enter symbol dataType offset

void CNoyInterpreter::enter(string id, string dataType, string size) {
	sTable.enter(id, dataType, 0, atoi(size.c_str()));
	ip++;
}

// assign:	arg2 := arg1

void CNoyInterpreter::assign(string arg1, string arg2, string arg3) {
	float val = sTable.getValue(arg1);
	if (sTable.getType(arg2) == "buo") {
		int temp = val;
		sTable.setValue(arg2, temp);
	} else
		sTable.setValue(arg2, val);
	ip++;
}

// unot:	arg2 := !arg1

void CNoyInterpreter::unot(string arg1, string arg2, string arg3) {

	float val = sTable.getValue(arg1);
	cout << "not " << val << endl;
	sTable.setValue(arg2, !val);
	ip++;
}

//add: arg3:= arg1+arg2

void CNoyInterpreter::add(string arg1, string arg2, string arg3) {

	if (sTable.getType(arg1) == "buo" && sTable.getType(arg2) == "buo")
		sTable.updateType(arg3, "buo");

	if (sTable.getType(arg3) == "buo")
		sTable.setValue(arg3, (int) ((sTable.getType(arg1) == "buo" ? (int) sTable.getValue(arg1) :
		   sTable.getValue(arg1)) + (sTable.getType(arg2) == "buo" ? (int) sTable.getValue(arg2) :
		   sTable.getValue(arg2))));
	else
		sTable.setValue(arg3, ((sTable.getType(arg1) == "buo" ? (int) sTable.getValue(arg1) :
		   sTable.getValue(arg1)) + (sTable.getType(arg2) == "buo" ? (int) sTable.getValue(arg2) :
		   sTable.getValue(arg2))));
	ip++;
}

//sub: arg3:= arg1-arg2

void CNoyInterpreter::sub(string arg1, string arg2, string arg3) {
	if (sTable.getType(arg1) == "buo" && sTable.getType(arg2) == "buo")
		sTable.updateType(arg3, "buo");

	if (sTable.getType(arg3) == "buo")
		sTable.setValue(arg3, (int) ((sTable.getType(arg1) == "buo" ? (int) sTable.getValue(arg1) :
		   sTable.getValue(arg1)) - (sTable.getType(arg2) == "buo" ? (int) sTable.getValue(arg2) :
		   sTable.getValue(arg2))));
	else
		sTable.setValue(arg3, ((sTable.getType(arg1) == "buo" ? (int) sTable.getValue(arg1) :
		   sTable.getValue(arg1)) - (sTable.getType(arg2) == "buo" ? (int) sTable.getValue(arg2) :
		   sTable.getValue(arg2))));
	ip++;
}

//lor: arg3:= arg1 || arg2

void CNoyInterpreter::lor(string arg1, string arg2, string arg3) {
	float val1 = sTable.getValue(arg1);
	float val2 = sTable.getValue(arg2);
	sTable.setValue(arg3, (val1 || val2));
	ip++;
}

//div: arg3:= arg1 / arg2

void CNoyInterpreter::div(string arg1, string arg2, string arg3) {

	if (sTable.getType(arg1) == "buo" && sTable.getType(arg2) == "buo")
		sTable.updateType(arg3, "buo");

	if (sTable.getValue(arg2) != 0) {
		if (sTable.getType(arg3) == "buo")
			sTable.setValue(arg3, (int) ((sTable.getType(arg1) == "buo" ? (int) sTable.getValue(arg1) :
			   sTable.getValue(arg1)) / (sTable.getType(arg2) == "buo" ? (int) sTable.getValue(arg2) :
			   sTable.getValue(arg2))));
		else
			sTable.setValue(arg3, ((sTable.getType(arg1) == "buo" ? (int) sTable.getValue(arg1) :
			   sTable.getValue(arg1)) / (sTable.getType(arg2) == "buo" ? (int) sTable.getValue(arg2) :
			   sTable.getValue(arg2))));
		ip++;
	} else {
		cout << "\nexception caught: divide by zero" << endl;
		ip = getSize() - 1; //return
	}

}

//mul: arg3:= arg1 * arg2

void CNoyInterpreter::mul(string arg1, string arg2, string arg3) {
	if (sTable.getType(arg1) == "buo" && sTable.getType(arg2) == "buo")
		sTable.updateType(arg3, "buo");

	if (sTable.getType(arg3) == "buo")
		sTable.setValue(arg3, (int) ((sTable.getType(arg1) == "buo" ? (int) sTable.getValue(arg1) :
		   sTable.getValue(arg1)) * (sTable.getType(arg2) == "buo" ? (int) sTable.getValue(arg2) :
		   sTable.getValue(arg2))));
	else
		sTable.setValue(arg3, ((sTable.getType(arg1) == "buo" ? (int) sTable.getValue(arg1) :
		   sTable.getValue(arg1)) * (sTable.getType(arg2) == "buo" ? (int) sTable.getValue(arg2) :
		   sTable.getValue(arg2))));
	ip++;
}

//mod: arg3:= arg1 % arg2

void CNoyInterpreter::mod(string arg1, string arg2, string arg3) {
	int val1 = sTable.getValue(arg1);
	int val2 = sTable.getValue(arg2);
	sTable.setValue(arg3, val1 % val2);
	ip++;
}

//land: arg3:= arg1 && arg2

void CNoyInterpreter::land(string arg1, string arg2, string arg3) {
	float val1 = sTable.getValue(arg1);
	float val2 = sTable.getValue(arg2);
	sTable.setValue(arg3, ((val1 && val2) != 0 ? 1 : 0));
	ip++;
}

//leq: arg3:= arg1 <= arg2

void CNoyInterpreter::leq(string arg1, string arg2, string arg3) {
	float val1 = sTable.getValue(arg1);
	float val2 = sTable.getValue(arg2);
	sTable.setValue(arg3, (val1 <= val2 ? 1 : 0));
	ip++;
}

//geq: arg3:= arg1 >= arg2

void CNoyInterpreter::geq(string arg1, string arg2, string arg3) {
	float val1 = sTable.getValue(arg1);
	float val2 = sTable.getValue(arg2);
	sTable.setValue(arg3, (val1 >= val2 ? 1 : 0));
	ip++;
}

//eq: arg3:= arg1 == arg2

void CNoyInterpreter::eq(string arg1, string arg2, string arg3) {
	float val1 = sTable.getValue(arg1);
	float val2 = sTable.getValue(arg2);
	sTable.setValue(arg3, (val1 == val2 ? 1 : 0));
	ip++;
}

//neq: arg3:= arg1 != arg2

void CNoyInterpreter::neq(string arg1, string arg2, string arg3) {
	float val1 = sTable.getValue(arg1);
	float val2 = sTable.getValue(arg2);
	sTable.setValue(arg3, (val1 != val2 ? 1 : 0));
	ip++;
}

//lt: arg3:= arg1 < arg2

void CNoyInterpreter::lt(string arg1, string arg2, string arg3) {
	float val1 = sTable.getValue(arg1);
	float val2 = sTable.getValue(arg2);
	sTable.setValue(arg3, (val1 < val2 ? 1 : 0));
	ip++;
}

//gt: arg3:= arg1 > arg2

void CNoyInterpreter::gt(string arg1, string arg2, string arg3) {
	float val1 = sTable.getValue(arg1);
	float val2 = sTable.getValue(arg2);
	sTable.setValue(arg3, (val1 > val2 ? 1 : 0));
	ip++;
}

//iftrue: arg1 goto arg2

void CNoyInterpreter::iftrue(string arg1, string arg2, string arg3) {
	float val1 = sTable.getValue(arg1);
	
	if (val1 != 0)
		ip = label[arg2];
	else
		ip++;
}

//goto: goto arg1

void CNoyInterpreter::go(string arg1, string arg2, string arg3) {
	ip = label[arg1];
}

// param: param x

void CNoyInterpreter::param(string arg1, string arg2, string arg3) {
	cStack.push(arg1);
	ip++;
}

// call: call p n 

void CNoyInterpreter::call(string arg1, string arg2, string arg3) {
	if (arg1 == "ipakita") {
		string a = cStack.top();

		if (a[0] == '\"') {
			a = a.substr(1, a.length() - 2);
			for (int i = 0; i < a.length(); i++) {
				if (a[i] == '\\' & a[i + 1] == 'n') {
					i++;
					cout << endl;
				} else
					cout << a[i];
			}

		} else if (sTable.search(a) != -1) {
			if (sTable.getType(a) == "buo")
				cout << (int) sTable.getValue(a);
			else
				cout << sTable.getValue(a);
		} else
			cout << a;
		cStack.pop();
	} else if (arg1 == "kumuha") {
		string id = cStack.top();
		cStack.pop();

		string temp;	
		cin >> temp;

		if (sTable.getType(id) == "buo")
			sTable.setValue(id, atoi(temp.c_str()));
		else
			sTable.setValue(id, atof(temp.c_str()));
	}

	ip++;
}

// halt:	halt

void CNoyInterpreter::halt(string arg1, string arg2, string arg3) {
}

// ret: ret x

void CNoyInterpreter::ret(string arg1, string arg2, string arg3) {
	ip = getSize() - 1;
}

void CNoyInterpreter::run() {
	ip = 0;
	while (code[ip].op != &CNoyInterpreter::halt)
		(this->*(code[ip].op))(code[ip].arg1, code[ip].arg2, code[ip].arg3);
}

void CNoyInterpreter::setLabel(string name, int i) {
	label[name] = i;
}
